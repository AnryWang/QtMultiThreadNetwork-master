#ifndef NETWORKDOWNLOADREQUEST_H
#define NETWORKDOWNLOADREQUEST_H

#include <QObject>
#include <QNetworkReply>
#include "NetworkRequest.h"

class QFile;
class QNetworkAccessManager;

//��������
class NetworkDownloadRequest : public NetworkRequest
{
	Q_OBJECT;

public:
	explicit NetworkDownloadRequest(QObject *parent = 0);
	~NetworkDownloadRequest();

public Q_SLOTS:
	void start() Q_DECL_OVERRIDE;
	void abort() Q_DECL_OVERRIDE;

	void onReadyRead();
	void onFinished();
	void onError(QNetworkReply::NetworkError);
	void onDownloadProgress(qint64 iReceived, qint64 iTotal);

private:
	//�����ļ������������ļ����ļ�������ɾ��
	bool createLocalFile();
	bool fileAccessible(QFile *pFile) const;
	bool removeFile(QFile *file);

private:
	std::unique_ptr<QFile> m_pFile;
	QNetworkAccessManager *m_pNetworkManager;
	QNetworkReply *m_pNetworkReply;
};

#endif // NETWORKDOWNLOADREQUEST_H