#ifndef _LLC_H
#define _LLC_H

#include <QSize>
#include <qdebug.h>

#include "abstractprotocol.h"

#include "llc.pb.h"
#include "ui_llc.h"

class LlcConfigForm : public QWidget, public Ui::llc
{
	Q_OBJECT
public:
	LlcConfigForm(QWidget *parent = 0);
};

class LlcProtocol : public AbstractProtocol
{
private:
	OstProto::Llc	data;
	static LlcConfigForm	*configForm;
	enum llcfield
	{
		llc_dsap = 0,
		llc_ssap,
		llc_ctl,

		llc_fieldCount
	};

public:
	LlcProtocol(Stream *parent = 0);
	virtual ~LlcProtocol();

	virtual void protoDataCopyInto(OstProto::Stream &stream);
	virtual void protoDataCopyFrom(const OstProto::Stream &stream);

	virtual QString name() const;
	virtual QString shortName() const;

	virtual int	fieldCount() const;

	virtual QVariant fieldData(int index, FieldAttrib attrib,
		   	int streamIndex = 0) const;
	virtual bool setFieldData(int index, const QVariant &value, 
			FieldAttrib attrib = FieldValue);

	virtual QWidget* configWidget();
	virtual void loadConfigWidget();
	virtual void storeConfigWidget();
};

#endif
