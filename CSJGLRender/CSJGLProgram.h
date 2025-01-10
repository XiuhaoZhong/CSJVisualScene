#ifndef __CSJGLPROGRAM_H__
#define __CSJGLPROGRAM_H__

#include <QString>
#include <QSharedPointer>
#include <QVector3D>
#include <QMatrix4x4>

class QOpenGLShaderProgram;

class CSJGLProgram {
public:
    CSJGLProgram();
    ~CSJGLProgram();

    bool initProgramWithSrcCode(QString& vertexSrc, QString& fragmentSrc);
    bool initProgramWidthShaderFile(QString& vertexFile, QString& fragmentFile);
    void useProgram();

    int getUniformLoc(QString& uniformName);

    void setUniformi(int location, int value);
    void setUniformf(int location, float value);
    void setUniformfv(int location, const QVector3D &value);
    void setUniformfm(int location, const QMatrix4x4 &value);


private:
    QSharedPointer<QOpenGLShaderProgram> m_spProgram;
};


#endif // __CSJGLPROGRAM_H__