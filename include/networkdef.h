#ifndef NETWORKDEF_H
#define NETWORKDEF_H

#include <QUrl>
#include <QEvent>
#include <QMap>
#include <QByteArray>
#include <QVariant>
#include <atomic>

#pragma pack(push, _CRT_PACKING)

// ��ģ��֧�ֵ�Э�飺HTTP(S)/FTP
// ��ģ��֧�ֵ�HTTPЭ�����󷽷���GET/POST/PUT/DELETE/HEAD

enum RequestType
{
	// Download��֧��http��ftp��
	eTypeDownload = 0,
	// Upload��֧��http��ftp��
	eTypeUpload,
	// GET��ʽ����֧��http��ftp��
	eTypeGet,
	// POST��ʽ���󣨽�֧��http��
	eTypePost,
	// PUT��ʽ����֧��http��ftp��
	eTypePut,
	// DELETE��ʽ���󣨽�֧��http��
	eTypeDelete,
	// HEAD��ʽ���󣨽�֧��http��
	eTypeHead,

	eTypeUnknown = -1,
};

#define ALL_TASK_ID 0xFFFF
//����ṹ
struct RequestTask
{
	// ��������ͣ��ϴ�/����/��������
	RequestType eType;

	// url
	// ע��: ftp�ϴ���url��ָ���ļ���.��"ftp://10.0.192.47:21/upload/test.zip", �ļ���������Ϊtest.zip.
	QUrl url;

	// case eTypeDownload:	���ص��ļ���ŵı���Ŀ¼. (����·�� or ���·��)
	// case eTypeUpload��	���ϴ����ļ�·��. (����·�� or ���·��)
	// case eTypePost��		post�Ĳ���. �磺"a=b&c=d".
	// case eTypePut��		put��������.
	QString strReqArg;

	// case eTypeDownload: ��ָ����strSaveFileName���򱣴���ļ�����strSaveFileName;���򣬸���url.
	QString strSaveFileName;

	// �����header��Ϣ
	//void QNetworkRequest::setRawHeader(const QByteArray &headerName, const QByteArray &value);
	QMap<QByteArray, QByteArray> mapRawHeader;

	// �Ƿ���ʾ���ȣ�Ĭ��Ϊfalse.
	bool bShowProgress;

	// ����ʧ�ܺ��Ƿ��ٳ�������һ�Σ�Ĭ��Ϊfalse.
	bool bTryAgainWhileFailed;

	// ��������ʧ��һ������ֹ��������Ĭ��Ϊtrue.
	bool bAbortBatchWhileOneFailed;

	// ���߳�����ģʽ(�������֧��)
	//	 ���߳�����ģʽ�£�һ���ļ��ɶ������ͨ��ͬʱ����.
	//	 ��Ҫ�Ȼ�ȡhttp head��Content-Length��������Ҫ��������֧��.
	bool bMultiDownloadMode;
	// n������ͨ��(Ĭ����5)(ȡֵ��Χ2-10)
	quint16 nDownloadThreadCount;

	// �û��Զ������ݣ������ڻش���
	QVariant varArg1;
	// �û��Զ������ݣ������ڻش���
	QVariant varArg2;
	// �û��Զ������ݣ������ڻش���
	QVariant varArg3;

	//////////////////////���ؽ�����ֶ�/////////////////////////////////////////////
	bool bFinished;	//��������
	bool bCancel;	//���ȡ��

	// �����Ƿ�ɹ�
	bool bSuccess;
	// ���󷵻ص�����/���صĴ�����Ϣ��.
	QByteArray bytesContent;

	// ����ID
	quint64 uiId;
	// ����ID (��������)
	quint64 uiBatchId;

	RequestTask()
	{
		uiId = 0;
		uiBatchId = 0;
		eType = eTypeUnknown;
		bFinished = false;
		bCancel = false;
		bSuccess = false;
		bShowProgress = false;
		bTryAgainWhileFailed = false;
		bAbortBatchWhileOneFailed = true;
		bMultiDownloadMode = false;
		nDownloadThreadCount = 5;
	}
};
Q_DECLARE_METATYPE(RequestTask);
typedef QVector<RequestTask> BatchRequestTask;


////////////////// Event ////////////////////////////////////////////////////
namespace QEventRegister
{
	template <class Type>
	int regiesterEvent(const Type& eventName)
	{
		typedef std::map<Type, int> TUserEventMap;
		static TUserEventMap s_mapUserEvent;

		TUserEventMap::const_iterator iter = s_mapUserEvent.find(eventName);
		if (iter != s_mapUserEvent.cend())
		{
			return iter->second;
		}

		int nEventType = QEvent::registerEventType();
		s_mapUserEvent[eventName] = nEventType;
		return nEventType;
	}
}

namespace NetworkEvent
{
	const QEvent::Type WaitForIdleThread = (QEvent::Type)QEventRegister::regiesterEvent(QLatin1String("WaitForIdleThread"));
	const QEvent::Type ReplyResult = (QEvent::Type)QEventRegister::regiesterEvent(QLatin1String("ReplyResult"));
	const QEvent::Type NetworkProgress = (QEvent::Type)QEventRegister::regiesterEvent(QLatin1String("NetworkProgress"));
}

//�ȴ������߳��¼�
class WaitForIdleThreadEvent : public QEvent
{
public:
	WaitForIdleThreadEvent() : QEvent(QEvent::Type(NetworkEvent::WaitForIdleThread)) {}
};

//֪ͨ����¼�
class ReplyResultEvent : public QEvent
{
public:
	ReplyResultEvent() : QEvent(QEvent::Type(NetworkEvent::ReplyResult)), bDestroyed(true) {}

	RequestTask request;
	bool bDestroyed;
};

//����/�ϴ������¼�
class NetworkProgressEvent : public QEvent
{
public:
	NetworkProgressEvent() : QEvent(QEvent::Type(NetworkEvent::NetworkProgress))
		, bDownload(true)
		, uiId(0)
		, uiBatchId(0)
		, iBtyes(0)
		, iTotalBtyes(0)
	{
	}

	bool bDownload;
	quint64 uiId;
	quint64 uiBatchId;
	qint64 iBtyes;
	qint64 iTotalBtyes;
};

#pragma pack(pop)

#endif ///NETWORKDEF_H