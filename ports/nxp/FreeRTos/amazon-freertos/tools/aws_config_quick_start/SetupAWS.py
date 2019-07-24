#!/usr/bin/env python

import argparse
import json
import thing
import policy
import certs
import misc
import boto3
import sys
import os

def check_aws_configuration():
    mysession = boto3.session.Session()
    if not (mysession._session._config['profiles']):
        print("AWS not configured. Please run `aws configure`.")
        sys.exit(1)


def prereq():
    with open('configure.json') as file:
        json_text = json.load(file)

    # Create a Thing
    thing_name = json_text['thing_name']
    thing_obj = thing.Thing(thing_name)
    if not thing_obj.create():

        # Create a Certificate
        cert_obj = certs.Certificate()
        result = cert_obj.create()

        # Store certId
        cert_id = result['certificateId']
        cert_id_filename = thing_name + '_cert_id_file'
        cert_id_file = open(cert_id_filename, 'w')
        cert_id_file.write(cert_id)
        cert_id_file_path = os.path.abspath(cert_id_filename)
        os.chmod(cert_id_file_path, 0o444)
        cert_id_file.close()

        # Store cert_pem as file
        cert_pem = result['certificatePem']
        cert_pem_filename = thing_name + '_cert_pem_file'
        cert_pem_file = open(cert_pem_filename, 'w')
        cert_pem_file.write(cert_pem)
        cert_pem_file_path = os.path.abspath(cert_pem_filename)
        os.chmod(cert_pem_file_path, 0o444)
        cert_pem_file.close()

        # Store private key PEM as file
        private_key_pem = result['keyPair']['PrivateKey']
        private_key_pem_filename = thing_name + '_private_key_pem_file'
        private_key_pem_file = open(private_key_pem_filename, 'w')
        private_key_pem_file.write(private_key_pem)
        private_key_pem_file_path = os.path.abspath(private_key_pem_filename)
        os.chmod(private_key_pem_file_path, 0o444)
        private_key_pem_file.close()

        # Create a Policy
        policy_document = misc.create_policy_document()
        policy_name = thing_name + '_amazon_freertos_policy'
        policy_obj = policy.Policy(policy_name, policy_document)
        policy_obj.create()

        # Attach certificate to Thing
        cert_obj.attach_thing(thing_name)

        # Attach policy to certificate
        cert_obj.attach_policy(policy_name)

def update_credential_file():
    with open('configure.json') as file:
        json_text = json.load(file)

    afr_source_dir = os.path.expanduser(json_text['afr_source_dir'])
    thing_name = json_text['thing_name']
    wifi_ssid = json_text['wifi_ssid']
    wifi_passwd = json_text['wifi_password']
    wifi_security = json_text['wifi_security']

    # Read cert_pem from file
    cert_pem_filename = thing_name + '_cert_pem_file'
    try:
        cert_pem_file = open(cert_pem_filename, 'r')
    except IOError:
        print("%s file not found. Run prerequisite step"%cert_pem_filename)
        sys.exit(1)
    else:
        cert_pem = cert_pem_file.read()

    # Read private_key_pem from file
    private_key_pem_filename = thing_name + '_private_key_pem_file'
    try:
        private_key_pem_file = open(private_key_pem_filename, 'r')
    except IOError:
        print("%s file not found. Run prerequisite step"%private_key_pem_filename)
        sys.exit(1)
    else:
        private_key_pem = private_key_pem_file.read()

    # Modify 'aws_clientcredential.h' file
    misc.update_client_credentials(
        afr_source_dir, thing_name, wifi_ssid, wifi_passwd, wifi_security)

    # Modify 'aws_clientcredential_keys.h' file
    misc.update_client_credential_keys(
        afr_source_dir, cert_pem, private_key_pem)

def delete_prereq():
    with open('configure.json') as file:
        json_text = json.load(file)

    # Delete Thing
    thing_name = json_text['thing_name']
    thing_obj = thing.Thing(thing_name)
    thing_obj.delete()

    # Delete certificate
    cert_id_filename = thing_name + '_cert_id_file'
    cert_id_file = open(cert_id_filename, 'r')
    cert_id =  cert_id_file.read()
    cert_obj = certs.Certificate(cert_id)
    cert_obj.delete()
    os.remove(cert_id_filename)

    # Delete cert_pem file and private_key_pem file
    cert_pem_filename = thing_name + '_cert_pem_file'
    private_key_pem_filename = thing_name + '_private_key_pem_file'
    os.remove(cert_pem_filename)
    os.remove(private_key_pem_filename)

    # Delete policy
    policy_name = thing_name + '_amazon_freertos_policy'
    policy_obj = policy.Policy(policy_name)
    policy_obj.delete()

def cleanup_creds():
    with open('configure.json') as file:
        json_text = json.load(file)

    afr_source_dir = os.path.expanduser(json_text['afr_source_dir'])
    # Cleanup modified 'aws_clientcredential.h' file
    misc.cleanup_client_credential_file(afr_source_dir)

    # Cleanup modified 'aws_clientcredential_keys.h' file
    misc.cleanup_client_credential_keys_file(afr_source_dir)

def setup():
    prereq()
    update_credential_file()

def cleanup():
    delete_prereq()
    cleanup_creds()

def list_certificates():
    client = boto3.client('iot')
    certs = client.list_certificates()['certificates']
    print(certs)

def list_things():
    client = boto3.client('iot')
    things = client.list_things()['things']
    print(things)

def list_policies():
    client = boto3.client('iot')
    policies = client.list_policies()['policies']
    print(policies)

if __name__ == "__main__":

    arg_parser = argparse.ArgumentParser()
    sub_arg_parser = arg_parser.add_subparsers(help='Available commands',
        dest='command')
    setup_parser = sub_arg_parser.add_parser('setup', help='setup aws iot')
    clean_parser = sub_arg_parser.add_parser('cleanup', help='cleanup aws iot')
    list_cert_parser = sub_arg_parser.add_parser('list_certificates',
        help='list certificates')
    list_thing_parser = sub_arg_parser.add_parser('list_things',
        help='list things')
    list_policy_parser = sub_arg_parser.add_parser('list_policies',
        help='list policies')
    prereq_parser = sub_arg_parser.add_parser('prereq',
        help='Setup Prerequisites for aws iot')
    update_creds = sub_arg_parser.add_parser('update_creds',
        help='Update credential files')
    delete_prereq_parser = sub_arg_parser.add_parser('delete_prereq',
        help='Delete prerequisites created')
    cleanup_creds_parser = sub_arg_parser.add_parser('cleanup_creds',
        help='Cleanup credential files')
    args = arg_parser.parse_args()

    check_aws_configuration()

    if args.command == 'setup':
        setup()
    elif args.command == 'cleanup':
        cleanup()
    elif args.command == 'list_certificates':
        list_certificates()
    elif args.command == 'list_things':
        list_things()
    elif args.command == 'list_policies':
        list_policies()
    elif args.command == 'prereq':
        prereq()
    elif args.command == 'update_creds':
        update_credential_file()
    elif args.command == 'delete_prereq':
        delete_prereq()
    elif args.command == 'cleanup_creds':
        cleanup_creds()
    else:
        print("Command does not exist")
