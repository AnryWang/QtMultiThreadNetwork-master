#pragma once

#include <memory>
#include <QString>

class QFile;
class QUrl;
namespace QMTNetwork {
    struct RequestTask;
}

class NetworkUtility
{
public:
    //���������ļ�
    static std::unique_ptr<QFile> createAndOpenFile(const QMTNetwork::RequestTask&, QString& errMessage);

    //���������д���ļ�
    static QString createSharedRWFileWin32(const QMTNetwork::RequestTask&, QString& errMessage, qint64 nDefaultFileSize = 0);

    //��ȡ�ļ�������
    static bool readFileContent(const QString& strFilePath, QByteArray& bytes, QString& errMessage);

    //��ȡ�����ļ�������ļ���
    static QString getDownloadFileSaveName(const QMTNetwork::RequestTask&);
    //��ȡ�����ļ������Ŀ¼
    static QString getDownloadFileSaveDir(const QMTNetwork::RequestTask&, QString& errMessage);

    static bool fileExists(QFile *pFile);
    static bool fileOpened(QFile *pFile);
    static bool removeFile(const QString& strFilePath, QString& errMessage);
    static QUrl currentRequestUrl(const QMTNetwork::RequestTask&);

private:
    NetworkUtility();
    ~NetworkUtility();
    NetworkUtility(const NetworkUtility &);
    NetworkUtility &operator=(const NetworkUtility &);
};

