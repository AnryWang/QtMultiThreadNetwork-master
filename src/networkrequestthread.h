#ifndef NETWORKREQUESTTHREAD_H
#define NETWORKREQUESTTHREAD_H

#include <QObject>
#include <QRunnable>
#include "NetworkDef.h"

class NetworkRequestThread: public QObject, public QRunnable
{
	Q_OBJECT;

public:
	explicit NetworkRequestThread(const RequestTask &, QObject *parent = 0);
	~NetworkRequestThread();

	//�̳г�����QRunnable��ִ�иú���������һ���µ��߳�
	//ִ��QThreadPool::start(QRunnable) ���� QThreadPool::tryStart(QRunnable)֮����Զ�����
	//������ɵĹ����ڸú�����ִ��
	virtual void run() Q_DECL_OVERRIDE;

	quint64 requsetId() const;
	quint64 batchId() const;
	const RequestTask task() {return m_task;}

	//�����¼�ѭ���Խ����߳�,���Զ���������ִ�е�����
	void quit();

Q_SIGNALS:
	void requestFinished(const RequestTask &);
	void exitEventLoop();

private Q_SLOTS:
	void onRequestFinished(bool, const QByteArray&);

private:
	RequestTask m_task;
};

#endif //CHQHTTPREQUESTTHREAD_H