#include "cameraresolution.h"
#include "camera.h"

#ifdef Q_OS_SYMBIAN
#include <ecam.h>
#include <NewFileServiceClient.h>
#include <AiwServiceHandler.h>
#include <AiwCommon.hrh>
#include <AiwGenericParam.hrh>
#endif

#include <QDebug>

Camera::Camera() : CPlugin(){
    PluginRegistry::getRegistry()->registerPlugin( "com.cordova.Camera", this );
}

Camera::~Camera()
{
    qDeleteAll(m_supportedResolutionObjects);
}

void Camera::init(){
    getSupportedResolutions();
}

void Camera::getPicture( int scId, int ecId, QVariantMap p_options){
    Q_UNUSED(ecId);
    Q_UNUSED(p_options);

    // Should we select the best resolution here?
    QString callbackArguments = newImageFile(m_supportedResolutions.at(0).width(),m_supportedResolutions.at(0).height());

    this->callback( scId, callbackArguments );
}

QString Camera::newImageFile(int width, int height)
{
#ifdef Q_OS_SYMBIAN
    QString filename;
    TRAPD(err, filename = symbianCapture(width, height));
    if (err != KErrNone)
        emit error(err);
    return filename;
#else
    return DUMMY_IMAGE;
#endif
}

QUrl Camera::newImageUrl(int width, int height)
{
    QString filename = newImageFile(width, height);
    if (filename.isEmpty())
        return QUrl();
    else
        return QUrl::fromLocalFile(filename);
}


QList<QSize> Camera::supportedResolutions()
{
    if (m_supportedResolutions.isEmpty())
        getSupportedResolutions();
    return m_supportedResolutions;
}

void Camera::getSupportedResolutions()
{
#ifdef Q_OS_SYMBIAN
    TRAPD(err, symbianEnumerateResolutions());
#else
    m_supportedResolutions.append(QSize(DUMMY_WIDTH, DUMMY_HEIGHT));
    m_supportedResolutionObjects.append(new CameraResolution(DUMMY_WIDTH, DUMMY_HEIGHT));
#endif
}

QList<QObject*> Camera::supportedResolutionObjects()
{
    if (m_supportedResolutionObjects.isEmpty())
        getSupportedResolutions();
    return m_supportedResolutionObjects;
}

#ifdef Q_OS_SYMBIAN
QString Camera::symbianCapture(int width, int height)
{
    CNewFileServiceClient* fileClient = NewFileServiceFactory::NewClientL();
    CleanupStack::PushL(fileClient);

    CDesCArray* fileNames = new (ELeave) CDesCArrayFlat(1);
    CleanupStack::PushL(fileNames);

    CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();

    TSize resolution = TSize(width, height);
    TPckgBuf<TSize> buffer( resolution );
    TAiwVariant resolutionVariant( buffer );
    TAiwGenericParam param( EGenericParamResolution, resolutionVariant );
    paramList->AppendL( param );

    const TUid KUidCamera = { 0x101F857A }; // Camera UID for S60 5th edition

    TBool result = fileClient->NewFileL( KUidCamera, *fileNames, paramList,
                               ENewFileServiceImage, EFalse );

    QString ret;

    if (result) {
        TPtrC fileName=fileNames->MdcaPoint(0);
        ret = QString((QChar*) fileName.Ptr(), fileName.Length());
    }
     qDebug() << ret;
    CleanupStack::PopAndDestroy(3);

    return ret;
}

void Camera::symbianEnumerateResolutions()
{
    CCamera* camera = CCamera::NewL(*this, 0);
    TCameraInfo info;
    camera->CameraInfo(info);
    int resolutionCount = info.iNumImageSizesSupported;
    qDebug() << resolutionCount;
    for (int i=0; i < resolutionCount; i++) {
        TSize size;
        camera->EnumerateCaptureSizes(size, i, CCamera::EFormatExif);
        qDebug() << size.iWidth << size.iHeight;
        m_supportedResolutions.append(QSize(size.iWidth, size.iHeight));
        m_supportedResolutionObjects.append(new CameraResolution(size.iWidth, size.iHeight));
    }
    qDebug() << m_supportedResolutionObjects.length();
    delete camera;

}
#endif

