#include "networkrunnable.h"
#include <QDebug>
#include <QEventLoop>
#include <QCoreApplication>
#include "Log4cplusWrapper.h"
#include "classmemorytracer.h"
#include "networkrequest.h"
#include "networkmanager.h"


NetworkRunnable::NetworkRunnable(const RequestTask &task, QObject *parent)
	: QObject(parent)
	, m_task(task)
{
	TRACE_CLASS_CONSTRUCTOR(NetworkRunnable);
	setAutoDelete(false);
}

NetworkRunnable::~NetworkRunnable()
{
	TRACE_CLASS_DESTRUCTOR(NetworkRunnable);
}

void NetworkRunnable::run()
{
	RequestTask task = m_task;
    std::unique_ptr<NetworkRequest> pRequest = nullptr;
	
	bool bQuit = false;
	QEventLoop loop;

	try
	{
		connect(this, &NetworkRunnable::exitEventLoop, &loop, [&bQuit, &loop, &pRequest]() {
			loop.quit();
			bQuit = true;
			if (nullptr != pRequest.get())
			{
				pRequest->disconnect();
				pRequest->abort();
				pRequest.reset();
			}
		}, Qt::QueuedConnection);

		if (!bQuit)
		{
            pRequest = std::move(NetworkRequestFactory::create(task.eType));
			
			if (pRequest.get())
			{
				connect(pRequest.get(), SIGNAL(requestFinished(bool, const QByteArray&)),
						this, SLOT(onRequestFinished(bool, const QByteArray&)));
				pRequest->setRequestTask(task);
				pRequest->start();
			}
			else
			{
				qWarning() << QString("Unsupported type(%1) ----").arg(task.eType) << task.url.url();
				LOG_ERROR("Unsupported type(" << task.eType << ")  ---- " << task.url.url().toStdWString());

				task.bSuccess = false;
				emit requestFinished(task);
				emit aboutToDelete();
			}
			loop.exec();
		}
	}
	catch(std::exception* e)
	{
        qCritical() << "NetworkRunnable::run() exception:" << QString::fromUtf8(e->what());
		LOG_ERROR("NetworkRunnable::run() exception: " << e->what());
	}
	catch (...)
	{
        qCritical() << "NetworkRunnable::run() exception:" << GetLastError();
		LOG_ERROR("NetworkRunnable::run() exception: " << GetLastError());
	}

	if (pRequest.get())
	{
		pRequest->abort();
		pRequest.reset();
	}
}

quint64 NetworkRunnable::requsetId() const
{
	return m_task.uiId;
}

quint64 NetworkRunnable::batchId() const
{
	return m_task.uiBatchId;
}

void NetworkRunnable::quit()
{
	disconnect(this, SIGNAL(requestFinished(const RequestTask &)),
			   NetworkManager::globalInstance(), SLOT(onRequestFinished(const RequestTask &)));
	emit exitEventLoop();
}

void NetworkRunnable::onRequestFinished(bool bSuccess, const QByteArray& bytesContent)
{
	RequestTask task = m_task;
	task.bSuccess = bSuccess;
	task.bytesContent = bytesContent;
	emit requestFinished(task);
}