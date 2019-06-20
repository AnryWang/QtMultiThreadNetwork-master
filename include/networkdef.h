#ifndef NETWORKDEF_H
#define NETWORKDEF_H

#include <QUrl>
#include <QEvent>
#include <QMap>
#include <QByteArray>
#include <QVariant>

#pragma pack(push, _CRT_PACKING)

// ��ģ��֧�ֵ�Э�飺HTTP(S)/FTP
// ��ģ��֧�ֵ�HTTP(s)Э�����󷽷���GET/POST/PUT/DELETE/HEAD

enum RequestType
{
    // Download��֧��http(s)��ftp��
    eTypeDownload = 0,
    // Multi-Thread Download��֧��http(s)��
    eTypeMTDownload = 1,
    // Upload��֧��http(s)��ftp��
    eTypeUpload = 2,
    // GET��ʽ����֧��http(s)��ftp��
    eTypeGet = 3,
    // POST��ʽ����֧��http(s)��
    eTypePost = 4,
    // PUT��ʽ����֧��http(s)��ftp��
    eTypePut = 5,
    // DELETE��ʽ����֧��http(s)��
    eTypeDelete = 6,
    // HEAD��ʽ����֧��http(s)��
    eTypeHead = 7,

    eTypeUnknown = -1,
};

//����ṹ
struct RequestTask
{
    enum
    {
        ALL_TASK = 0xFFFFFFFF
    };

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

    bool bRemoveFileWhileExist;

    // ����ʧ�ܺ��Ƿ��ٳ�������һ�Σ�Ĭ��Ϊfalse.
    bool bTryAgainWhileFailed;

    // ��������ʧ��һ������ֹ��������Ĭ��Ϊfalse.
    bool bAbortBatchWhileOneFailed;

    // ���ļ����߳�����ģʽ(�������֧��) ע��eTypeΪeTypeMTDownloadʱ��Ч
    //	 ���߳�����ģʽ�£�һ���ļ��ɶ������ͨ��ͬʱ����.
    //	 ��Ҫ�Ȼ�ȡhttp head��Content-Length��������Ҫ��������֧��.
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
        bRemoveFileWhileExist = false;
        bTryAgainWhileFailed = false;
        bAbortBatchWhileOneFailed = false;
        nDownloadThreadCount = 5;
    }
};
Q_DECLARE_METATYPE(RequestTask);
typedef QVector<RequestTask> BatchRequestTask;


inline const QString getTypeString(RequestType eType)
{
    QString strType;
    switch (eType)
    {
    case eTypeDownload:
    {
        strType = QStringLiteral("����");
    }
    break;
    case eTypeMTDownload:
    {
        strType = QStringLiteral("MT����");
    }
    break;
    case eTypeUpload:
    {
        strType = QStringLiteral("�ϴ�");
    }
    break;
    case eTypeGet:
    {
        strType = QStringLiteral("GET");
    }
    break;
    case eTypePost:
    {
        strType = QStringLiteral("POST");
    }
    break;
    case eTypePut:
    {
        strType = QStringLiteral("PUT");
    }
    break;
    case eTypeDelete:
    {
        strType = QStringLiteral("DELETE");
    }
    break;
    case eTypeHead:
    {
        strType = QStringLiteral("HEAD");
    }
    break;
    default:
        break;
    }
    return strType;
}


////////////////// Event ////////////////////////////////////////////////////
namespace QEventRegister
{
    template <typename T>
    int regiesterEvent(const T& eventName)
    {
        static_assert(std::is_integral<T>::value
            || std::is_base_of<std::string, T>::value
            || std::is_base_of<std::wstring, T>::value
            || std::is_base_of<QString, T>::value
            || std::is_base_of<QLatin1String, T>::value, "T is a unsupported Type.");

        typedef std::map<T, int> UserEventMap;
        static UserEventMap s_mapUserEvent;

        UserEventMap::const_iterator iter = s_mapUserEvent.find(eventName);
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