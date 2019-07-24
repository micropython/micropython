#!/usr/bin/env python

import os
import boto3
import shutil
import sys
import json


def describe_endpoint():
    client = boto3.client('iot')
    endpoint = client.describe_endpoint()
    return endpoint['endpointAddress']

def get_account_id():
    client = boto3.client('sts')
    aws_account_id = client.get_caller_identity()['Account']
    return aws_account_id.strip('\n')

def get_aws_region():
    my_session = boto3.session.Session()
    aws_region = my_session.region_name
    return aws_region.strip('\n')

def create_policy_document():
    this_file_directory = os.getcwd()
    policy_document = os.path.join(this_file_directory, 'policy_document.templ')
    region_name = str(get_aws_region())
    aws_account_id  = str(get_account_id())
    with open(policy_document) as policy_document_file:
        policy_document_text = policy_document_file.read()

    # Replace Account ID and AWS Region
    policy_document_text = policy_document_text.replace(
        '<aws-region>', region_name)
    policy_document_text = policy_document_text.replace(
        '<aws-account-id>', aws_account_id)

    return policy_document_text

def format_credential_keys_text(credentialText):
    credentialTextLines = credentialText.split('\n')
    formattedCredentialTextLines = []

    for credentialTextLine in credentialTextLines:
        if credentialTextLine.strip():
            formattedCredentialTextLine = '"' + credentialTextLine + '\\n"'
            formattedCredentialTextLines.append(formattedCredentialTextLine)

    formattedCredentialText = '\\\n'.join(formattedCredentialTextLines)
    return formattedCredentialText

def update_client_credentials(afr_source_dir, thing_name, wifi_ssid, wifi_passwd, wifi_security):
    file_to_modify = os.path.join(
        afr_source_dir, 'demos', 'common', 'include', 'aws_clientcredential.h')

    filename = "aws_clientcredential.templ"
    with open(filename,'r') as template_file:
        file_text = template_file.read()
        new_text = file_text.replace("<WiFiSSID>", "\"" + wifi_ssid + "\"")
        new_text = new_text.replace("<WiFiPasswd>", "\"" + wifi_passwd + "\"")
        new_text = new_text.replace("<WiFiSecurity>", wifi_security)
        new_text = new_text.replace("<IOTThingName>", "\"" + thing_name + "\"")
        new_text = new_text.replace(
            "<IOTEndpoint>", "\"" + describe_endpoint() + "\"")
        header_file = open(str(file_to_modify),'w')
        header_file.write(new_text)
        header_file.close()

def update_client_credential_keys(afr_source_dir, client_certificate_pem, client_private_key_pem):
    file_to_modify = os.path.join(
        afr_source_dir, 'demos', 'common', 'include', 'aws_clientcredential_keys.h')

    filename = "aws_clientcredential_keys.templ"
    with open(filename,'r') as template_file:
        file_text = template_file.read()
        new_text = file_text.replace("<ClientCertificatePEM>",
                format_credential_keys_text(client_certificate_pem))
        new_text = new_text.replace("<ClientPrivateKeyPEM>",
                format_credential_keys_text(client_private_key_pem))
        header_file = open(str(file_to_modify),'w')
        header_file.write(new_text)
        header_file.close()

def cleanup_client_credential_file(afr_source_dir):
    client_credential_file = os.path.join(
        afr_source_dir, 'demos', 'common', 'include', 'aws_clientcredential.h')

    endpoint_string = "Paste AWS IoT Broker endpoint here."
    wifi_ssid_string = "Paste Wi-Fi SSID here."
    wifi_passwd_string = "Paste Wi-Fi password here."
    thing_name_string = "Paste AWS IoT Thing name here."
    wifi_security_string = "eWiFiSecurityWPA2"

    filename = "aws_clientcredential.templ"
    with open(filename,'r') as template_file:
        file_text = template_file.read()
        new_text = file_text.replace(
            "<WiFiSSID>", "\"" + wifi_ssid_string + "\"")
        new_text = new_text.replace(
            "<WiFiPasswd>", "\"" + wifi_passwd_string + "\"")
        new_text = new_text.replace(
            "<WiFiSecurity>", wifi_security_string)
        new_text = new_text.replace(
            "<IOTThingName>", "\"" + thing_name_string + "\"")
        new_text = new_text.replace(
            "<IOTEndpoint>", "\"" + endpoint_string + "\"")
        header_file = open(str(client_credential_file),'w')
        header_file.write(new_text)
        header_file.close()


def cleanup_client_credential_keys_file(afr_source_dir):
    client_credential_keys_file = os.path.join(
        afr_source_dir, 'demos', 'common', 'include', 'aws_clientcredential_keys.h')

    certificate_pem_string = "Paste client certificate here."
    private_key_pem_string = "Paste client private key here."

    filename = "aws_clientcredential_keys.templ"
    with open(filename, 'r') as template_file:
        file_text = template_file.read()
        new_text = file_text.replace(
            "<ClientCertificatePEM>", "\"" + certificate_pem_string + "\"")
        new_text = new_text.replace(
            "<ClientPrivateKeyPEM>", "\"" + private_key_pem_string + "\"")
        header_file = open(str(client_credential_keys_file),'w')
        header_file.write(new_text)
        header_file.close()
