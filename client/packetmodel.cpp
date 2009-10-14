#include <QHostAddress>

#include "packetmodel.h"
#include "../common/protocollistiterator.h"
#include "../common/abstractprotocol.h"

PacketModel::PacketModel(QObject *parent)
{
}

void PacketModel::setSelectedProtocols(ProtocolListIterator &iter)
{
	QList<const AbstractProtocol*> currentProtocols;

	iter.toFront();
	while (iter.hasNext())
		currentProtocols.append(iter.next());

	if (mSelectedProtocols != currentProtocols)
	{
		mSelectedProtocols = currentProtocols;
		reset();
	}
}

int PacketModel::rowCount(const QModelIndex &parent) const
{
	IndexId		parentId;

	// qDebug("in %s", __FUNCTION__);

	// Parent == Invalid i.e. Invisible Root.
	// ==> Children are Protocol (Top Level) Items
	if (!parent.isValid())
		return mSelectedProtocols.size();

	// Parent - Valid Item
	parentId.w = parent.internalId();
	switch(parentId.ws.type)
	{
	case  ITYP_PROTOCOL:
		return mSelectedProtocols.at(parentId.ws.protocol)->frameFieldCount();
	case  ITYP_FIELD: 
		return 0;
	default:
		qWarning("%s: Unhandled ItemType", __FUNCTION__);
	}

	Q_ASSERT(1 == 0);	// Unreachable code
	qWarning("%s: Catch all - need to investigate", __FUNCTION__);
	return 0; // catch all
}

int PacketModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

QModelIndex PacketModel::index(int row, int col, const QModelIndex &parent) const
{
	QModelIndex	index;
	IndexId	id, parentId;

	if (!hasIndex(row, col, parent))
		goto _exit;

	// Parent is Invisible Root
	// Request for a Protocol Item
	if (!parent.isValid())
	{
		id.w = 0;
		id.ws.type = ITYP_PROTOCOL;
		id.ws.protocol = row;
		index = createIndex(row, col, id.w);
		goto _exit;
	}

	// Parent is a Valid Item
	parentId.w = parent.internalId();
	id.w = parentId.w;
	switch(parentId.ws.type)
	{
	case  ITYP_PROTOCOL:
		id.ws.type = ITYP_FIELD;
		index = createIndex(row, col, id.w);
		goto _exit;

	case  ITYP_FIELD: 
		Q_ASSERT(1 == 0);	// Unreachable code
		goto _exit;

	default:
		qWarning("%s: Unhandled ItemType", __FUNCTION__);
	}

	Q_ASSERT(1 == 0);	// Unreachable code

_exit:
	return index;
}

QModelIndex PacketModel::parent(const QModelIndex &index) const
{
	QModelIndex	parentIndex;
	IndexId		id, parentId;

	if (!index.isValid())
		return QModelIndex();

	id.w = index.internalId();
	parentId.w = id.w;
	switch(id.ws.type)
	{
	case  ITYP_PROTOCOL:
		// return invalid index for invisible root
		goto _exit;

	case  ITYP_FIELD: 
		parentId.ws.type = ITYP_PROTOCOL;
		parentIndex = createIndex(id.ws.protocol, 0, parentId.w);
		goto _exit;

	default:
		qWarning("%s: Unhandled ItemType", __FUNCTION__);
	}

	Q_ASSERT(1 == 1); // Unreachable code

_exit:
	return parentIndex;
}

QVariant PacketModel::data(const QModelIndex &index, int role) const
{
	IndexId			id;

	if (!index.isValid())
		return QVariant();

	id.w = index.internalId();

	// FIXME(HI): Relook at this completely
	if (role == Qt::UserRole)
	{
		switch(id.ws.type)
		{
		case  ITYP_PROTOCOL:
			qDebug("*** %d/%d", id.ws.protocol, mSelectedProtocols.size());
			return	mSelectedProtocols.at(id.ws.protocol)->
				protocolFrameValue();

		case  ITYP_FIELD: 
			return	mSelectedProtocols.at(id.ws.protocol)->fieldData(
				index.row(), AbstractProtocol::FieldFrameValue);

		default:
			qWarning("%s: Unhandled ItemType", __FUNCTION__);
		}
		return QByteArray(); 
	}

	// FIXME: Use a new enum here instead of UserRole
	if (role == (Qt::UserRole+1))
	{
		switch(id.ws.type)
		{
		case  ITYP_PROTOCOL:
			return	mSelectedProtocols.at(id.ws.protocol)->
				protocolFrameValue().size();

		case  ITYP_FIELD: 
			return	mSelectedProtocols.at(id.ws.protocol)->fieldData(
				index.row(), AbstractProtocol::FieldBitSize);

		default:
			qWarning("%s: Unhandled ItemType", __FUNCTION__);
		}
		return QVariant(); 
	}

	if (role != Qt::DisplayRole)
		return QVariant();

	switch(id.ws.type)
	{
	case  ITYP_PROTOCOL:
		return QString("%1 (%2)")
			.arg(mSelectedProtocols.at(id.ws.protocol)->shortName())
			.arg(mSelectedProtocols.at(id.ws.protocol)->name());

	case  ITYP_FIELD: 
		return	mSelectedProtocols.at(id.ws.protocol)->fieldData(index.row(),
				AbstractProtocol::FieldName).toString() + QString(" : ") +
				mSelectedProtocols.at(id.ws.protocol)->fieldData(index.row(),
				AbstractProtocol::FieldTextValue).toString();

	default:
		qWarning("%s: Unhandled ItemType", __FUNCTION__);
	}

	Q_ASSERT(1 == 1); // Unreachable code

	return QVariant();
}
