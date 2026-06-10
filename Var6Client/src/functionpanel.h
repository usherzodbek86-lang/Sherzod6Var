#pragma once
#include <QWidget>
#include <QStackedWidget>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include "resulttable.h"

/**
 * @brief Панель с четырьмя вкладками для функций варианта:
 *        RSA, SHA-1, Метод Ньютона, Стеганография (WAV).
 */
class FunctionPanel : public QWidget {
    Q_OBJECT
public:
    explicit FunctionPanel(QWidget *parent = nullptr);

private slots:
    void onResponse(const QJsonObject &resp);

    // RSA
    void onRsaEncrypt();
    void onRsaDecrypt();

    // SHA-1
    void onSha1Hash();

    // Newton
    void onNewtonSolve();

    // Steganography
    void onEmbedAudio();
    void onExtractAudio();

private:
    // Построение вкладок
    QWidget *buildRsaTab();
    QWidget *buildSha1Tab();
    QWidget *buildNewtonTab();
    QWidget *buildStegoTab();
    void     setupUi();

    // ── RSA ──
    QTextEdit   *m_rsaInput;
    QTextEdit   *m_rsaEncrypted;
    QTextEdit   *m_rsaKeys;
    QLabel      *m_rsaStatus;

    // ── SHA-1 ──
    QLineEdit   *m_sha1Input;
    QLabel      *m_sha1Result;

    // ── Newton ──
    QComboBox   *m_newtonFunc;
    QLineEdit   *m_newtonX0;
    ResultTable *m_newtonTable;
    QLabel      *m_newtonRoot;

    // ── Stego ──
    QLabel      *m_stegoFile;
    QLineEdit   *m_stegoMessage;
    QLabel      *m_stegoStatus;
    QString      m_stegoFilePath;
    QString      m_pendingAction; // "embed" | "extract"

    QByteArray   m_lastWavData;  // последний загруженный WAV
};
