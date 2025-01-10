#include "CSJGLProgram.h"

#include <QOpenGLShaderProgram>

CSJGLProgram::CSJGLProgram()
    : m_spProgram(QSharedPointer<QOpenGLShaderProgram>::create()) {

}

CSJGLProgram::~CSJGLProgram() {

}

bool CSJGLProgram::initProgramWithSrcCode(QString& vertexSrc, 
                                          QString& fragmentSrc) {
    bool res;
    res = m_spProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                               vertexSrc);
    if (!res) {
        QString errorLog = m_spProgram->log();
        return false;
    }

    res = m_spProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                               fragmentSrc);
    if (!res) {
        QString errorLog = m_spProgram->log();
        return false;
    }

    res = m_spProgram->link();
    if (!res) {
        QString errorLog = m_spProgram->log();
        return false;
    }
    
    return true;
}

bool CSJGLProgram::initProgramWidthShaderFile(QString& vertexFile,
                                              QString& fragmentFile) {
    bool res;
    res = m_spProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                               vertexFile);
    if (!res) {
        QString errorLog = m_spProgram->log();
        return false;
    }

    res = m_spProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                               fragmentFile);
    if (!res) {
        QString errorLog = m_spProgram->log();
        return false;
    }

    res = m_spProgram->link();
    if (!res) {
        QString errorLog = m_spProgram->log();
        return false;
    }
    
    return true;
}

void CSJGLProgram::useProgram() {
    bool res = m_spProgram->bind();
    if (!res) {
        QString errorLog = m_spProgram->log();
        // TODO: record the error log.
    }
}

int CSJGLProgram::getUniformLoc(QString& uniformName) {
    return m_spProgram->uniformLocation(uniformName);
}

void CSJGLProgram::setUniformi(int location, int value) {
    m_spProgram->setUniformValue(location, value);
}

void CSJGLProgram::setUniformf(int location, float value) {
    m_spProgram->setUniformValue(location,  value);
}

void CSJGLProgram::setUniformfv(int location, const QVector3D& value) {
    m_spProgram->setUniformValue(location, value);
}

void CSJGLProgram::setUniformfm(int location, const QMatrix4x4& value) {
    m_spProgram->setUniformValue(location, value);
}