
// Check for the various File API support.
if (window.File && window.FileReader && window.FileList && window.Blob) {
    // Success! All the File APIs are supported.
} else {
    alert('Please use a web browser that supports HTML5 file APIs.');
}

function formatCredentialText(credentialText) {
    // Replace any CR/LF pairs with a newline character.
    credentialText = credentialText.replace(/\r\n/g, "\n");

    // Add line endings for C-language variable declaration.
    credentialText = credentialText.replace(/\n/g, "\\n\"\\\n\"");

    // Remove '\n"' from the last line of the declaration and add a semicolon.
    credentialText = credentialText.slice(0, -6) + "\"\n";
    return credentialText;
}

function generateCertificateConfigurationHeader() {
    pemCertificateText = "";
    pemPrivateKeyText = "";
    filename = "aws_clientcredential_keys.h";

    var readerCertificate = new FileReader();
    var readerPrivateKey = new FileReader();

    // Define a handler to create appropriate client certificate file text.
    readerCertificate.onload = (function (e) {
        pemCertificateText = e.target.result;

        // Add C-language variable declaration plus EOL formatting.
        pemCertificateText = "#define keyCLIENT_CERTIFICATE_PEM \\\n" + "\"" +
            formatCredentialText(pemCertificateText);

        // Because this is async, read next file inline.
        readerPrivateKey.readAsText(pemInputFilePrivateKey.files[0]);
    });

    // Define a handler to create appropriate private key file text.
    readerPrivateKey.onload = (function (e) {
        pemPrivateKeyText = e.target.result;

        // Add C-language variable declaration plus EOL formatting.
        pemPrivateKeyText = "#define keyCLIENT_PRIVATE_KEY_PEM \\\n" + "\"" +
            formatCredentialText(pemPrivateKeyText);

        // Concatenate results for the final output.
        outputText = header_begin_text +
            client_certificate_comment_text +
            pemCertificateText + "\n" +
            private_key_comment_text +
            pemPrivateKeyText + "\n" +
            jitr_certificate_text +
            header_end_text;

        // Because this is async, handle download generation inline.
        var downloadBlob = new Blob([outputText], { type: 'text/plain' });
        if (window.navigator.msSaveOrOpenBlob) {
            window.navigator.msSaveBlob(downloadBlob, filename);
        } else {
            var downloadLink = document.createElement('a');
            downloadLink.href = window.URL.createObjectURL(downloadBlob);
            downloadLink.download = filename;
            document.body.appendChild(downloadLink);
            downloadLink.click();
            document.body.removeChild(downloadLink);
        }
    });

    header_begin_text = 
"#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H\n\
#define AWS_CLIENT_CREDENTIAL_KEYS_H\n\
\n\
#include \<stdint.h\>\n\
\n";

    private_key_comment_text =
"\/\*\n\
\ * PEM-encoded client private key.\n\
\ *\n\
\ * Must include the PEM header and footer:\n\
\ * \"-----BEGIN RSA PRIVATE KEY-----\\n\"\\\n\
\ * \"...base64 data...\\n\"\\\n\
\ * \"-----END RSA PRIVATE KEY-----\"\n\
\ *\/\n";

    client_certificate_comment_text =
"\/\*\n\
\ * PEM-encoded client certificate.\n\
\ *\n\
\ * Must include the PEM header and footer:\n\
\ * \"-----BEGIN CERTIFICATE-----\\n\"\\\n\
\ * \"...base64 data...\\n\"\\\n\
\ * \"-----END CERTIFICATE-----\"\n\
\ *\/\n";

    jitr_certificate_text =
"\/\*\n\
\ * PEM-encoded Just-in-Time Registration (JITR) certificate (optional).\n\
\ *\n\
\ * If used, must include the PEM header and footer:\n\
\ * \"-----BEGIN CERTIFICATE-----\\n\"\\\n\
\ * \"...base64 data...\\n\"\\\n\
\ * \"-----END CERTIFICATE-----\"\n\
\ *\/\n\
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  NULL\n\
\n";

    header_end_text =
"\/\* The constants above are set to const char * pointers defined in aws_dev_mode_key_provisioning.c,\n\
\ * and externed here for use in C files.  NOTE!  THIS IS DONE FOR CONVENIENCE\n\
\ * DURING AN EVALUATION PHASE AND IS NOT GOOD PRACTICE FOR PRODUCTION SYSTEMS\n\
\ * WHICH MUST STORE KEYS SECURELY. *\/\n\
extern const char clientcredentialCLIENT_CERTIFICATE_PEM\[\];\n\
extern const char* clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;\n\
extern const char clientcredentialCLIENT_PRIVATE_KEY_PEM\[\];\n\
extern const uint32_t clientcredentialCLIENT_CERTIFICATE_LENGTH;\n\
extern const uint32_t clientcredentialCLIENT_PRIVATE_KEY_LENGTH;\n\
\n\
#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H *\/\n";

    //Start first async read - other calls are chained inline
    readerCertificate.readAsText(pemInputFileCertificate.files[0]);
}

