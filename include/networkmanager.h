/*
@Brief:			Qt multi-threaded network module
@Author:		vilas wang
@Contact:		QQ451930733|451930733@qq.com

The Qt multi-threaded network module is a wrapper of Qt Network module, and combine with thread-pool to realize multi-threaded networking.
- Multi-task concurrent(Each request task is executed in different threads).
- Both single request and batch request mode are supported.
- Big file multi-thread downloading supported. (The thread here refers to the download channel. Download speed is faster.)
- HTTP(S)/FTP protocol supported.
- Multiple request methods supported. (GET/POST/PUT/DELETE/HEAD)
- Asynchronous API.
- Thread-safe.

Note: You must call NetworkManager::initialize() before use, and call NetworkManager::unInitialize() before application quit.
That must be called in the main thread.
*/

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include "NetworkReply.h"
#include "NetworkDef.h"
#include "Network_Global.h"

class QEvent;
class NetworkManagerPrivate;
class NETWORK_EXPORT NetworkManager : public QObject
{
	Q_OBJECT;
	Q_DECLARE_PRIVATE(NetworkManager)

public:
	// ��ʼ���ͷ���ʼ�����������߳��е���
	static void initialize();
	static void unInitialize();
	// �Ƿ��Ѿ���ʼ��
	static bool isInitialized();

	static NetworkManager* globalInstance();
	static void deleteInstance();
	// �Ƿ��Ѿ�ʵ����
	static bool isInstantiated();

public:
	// NetworkReply�����������������Զ����٣��û�������������
	// ��ӵ�����������������nullptr����ʾurl��Ч��
	NetworkReply *addRequest(RequestTask& task);

	// ���������������
	NetworkReply *addBatchRequest(BatchRequestTask& tasks, quint64 &uiBatchId);

	// ֹͣ���е���������
	void stopAllRequest();
	// ָֹͣ��batchid��������������
	void stopBatchRequests(quint64 uiBatchId);
	// ֹͣĳ����������
	void stopRequest(quint64 uiTaskId);

	// �����̳߳�����߳�������1-16��, Ĭ��5�̣߳�
	bool setMaxThreadCount(int iMax);
	int maxThreadCount();

Q_SIGNALS:
	void errorMessage(const QString& error);
	void batchRequestFinished(quint64 uiBatchId, bool bAllSuccess);

	// Progress
	void downloadProgress(quint64 uiRequestId, qint64 iBytesDownload, qint64 iBytesTotal);
	void uploadProgress(quint64 uiRequestId, qint64 iBytesUpload, qint64 iBytesTotal);
	void batchDownloadProgress(quint64 uiBatchId, qint64 iBytesDownload);
	void batchUploadProgress(quint64 uiBatchId, qint64 iBytesUpload);

private Q_SLOTS:
	void onRequestFinished(const RequestTask &);

public:
	bool event(QEvent *pEvent) Q_DECL_OVERRIDE;

private:
	NetworkManager(QObject *parent = 0);
	~NetworkManager();
	Q_DISABLE_COPY(NetworkManager);

private:
	void init();
	void fini();

	bool startAsRunnable(const RequestTask &task);

	// bDownload(false: upload)
	void updateProgress(quint64 uiRequestId, quint64 uiBatchId,
		qint64 iBytes, qint64 iTotalBytes, bool bDownload);

private:
	QScopedPointer<NetworkManagerPrivate> d_ptr;
	static NetworkManager* ms_pInstance;
	static bool ms_bIntialized;
};

#endif //NETWORKMANAGER_H