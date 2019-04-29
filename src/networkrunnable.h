#ifndef NETWORKRUNNABLE_H
#define NETWORKRUNNABLE_H

#include <QObject>
#include <QRunnable>
#include "NetworkDef.h"

class NetworkRunnable : public QObject, public QRunnable
{
	Q_OBJECT

public:
	explicit NetworkRunnable(const RequestTask &, QObject *parent = 0);
	~NetworkRunnable();

	//ִ��QThreadPool::start(QRunnable) ���� QThreadPool::tryStart(QRunnable)֮����Զ�����
	virtual void run() Q_DECL_OVERRIDE;

	quint64 requsetId() const;
	quint64 batchId() const;
	const RequestTask task() const { return m_task; }

	//�����¼�ѭ�����ͷ������̣߳�ʹ���ɿ���״̬,���һ��Զ���������ִ�е�����
	void quit();

Q_SIGNALS:
	void requestFinished(const RequestTask &);
	void exitEventLoop();
	void aboutToDelete();

private Q_SLOTS:
	void onRequestFinished(bool, const QByteArray&);

private:
	Q_DISABLE_COPY(NetworkRunnable);
	RequestTask m_task;
};

#endif //NETWORKRUNNABLE_H