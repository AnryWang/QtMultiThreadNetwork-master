#ifndef NETWORKBIGFLEDOWNLOADREQUEST_H
#define NETWORKBIGFLEDOWNLOADREQUEST_H

#include <QObject>
#include <QNetworkReply>
#include <QMutex>
#include <QPointer>
#include "NetworkRequest.h"

class QFile;
class QNetworkAccessManager;
class Downloader;

//���߳���������(������߳���ָ���ص�ͨ����һ���ļ����ֳɶ�����֣��ɶ������ͨ��ͬʱ����)
class NetworkMTDownloadRequest : public NetworkRequest
{
	Q_OBJECT;

public:
	explicit NetworkMTDownloadRequest(QObject *parent = 0);
	~NetworkMTDownloadRequest();

public Q_SLOTS:
	void start() Q_DECL_OVERRIDE;
	void abort() Q_DECL_OVERRIDE;

	void onSubPartFinished(int index, bool bSuccess, const QString& strErr);
	void onSubPartDownloadProgress(int index, qint64 bytesReceived, qint64 bytesTotal);
	void onGetFileSizeFinished();
	void onGetFileSizeError(QNetworkReply::NetworkError);

private:
	bool requestFileSize(QUrl url);
	//�����ļ������������ļ�
	bool createLocalFile();
	bool fileAccessible(QFile *pFile) const;
	bool removeFile(QFile *file);
	void startMTDownload();

private:
	std::shared_ptr<QFile> m_pFile;
	QNetworkAccessManager *m_pNetworkManager;
	QNetworkReply *m_pReply;

	QMap<int, std::shared_ptr<Downloader>> m_mapDownloader;
	int m_nThreadCount;//�ָ�ɶ��ٶ�����

	QUrl m_url;
	qint64 m_nFileSize;
	int m_nSuccessNum;
	int m_nFailedNum;

	struct ProgressData
	{
		qint64 bytesReceived;
		qint64 bytesTotal;
		ProgressData()
		{
			bytesReceived = 0;
			bytesTotal = 0;
		}
	};

	QMap<int, ProgressData> m_mapBytes;
	qint64 m_bytesTotal;
	qint64 m_bytesReceived;
};


//���������ļ������ļ���һ���֣�
class Downloader : public QObject
{
	Q_OBJECT

public:
	Downloader(int index, QObject *parent = 0);
	~Downloader();

	bool startDownload(const QUrl &url, 
		QFile* file,
		QNetworkAccessManager* pNetworkManager,
		qint64 startPoint	= 0, 
		qint64 endPoint		= -1,
		bool bShowProgress	= false);

	void abort();

Q_SIGNALS:
	void downloadFinished(int index, bool bSuccess, const QString& strErr);
	void downloadProgress(int index, qint64 bytesReceived, qint64 bytesTotal);

	public Q_SLOTS:
		void onFinished();
		void onReadyRead();
		void onError(QNetworkReply::NetworkError code);

private:
	QPointer<QNetworkAccessManager> m_pNetworkManager;
	QNetworkReply *m_pNetworkReply;
	QPointer <QFile> m_pFile;
	QUrl m_url;
	mutable QMutex m_mutex;

	bool m_bAbortManual;
	QString m_strError;

	const int m_nIndex;
	qint64 m_nHaveDoneBytes;
	qint64 m_nStartPoint;
	qint64 m_nEndPoint;
	bool m_bShowProgress;
};

#endif // NETWORKBIGFLEDOWNLOADREQUEST_H
