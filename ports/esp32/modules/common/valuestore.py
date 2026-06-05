## These functions allow you to easily load and save Python objects as json files in /config/.

def load(namespace='system', keyname=''):
    """
    Loads any type of Python primitive object from a json file.
    :param namespace:
    :param keyname:
    :return:
    """
    import ujson
    if namespace == '' or keyname is None:
        print('valuestore.load() called with empty namespace')
        return None
    if keyname == '' or keyname is None:
        print('valuestore.load() called with empty keyname')
        return None

    try:
        with open('/config/%s-%s.json' % (namespace, keyname), 'r') as file:
            return ujson.load(file)
    except BaseException as error:
        if 'ENOENT' not in str(error):
            print('Error reading cache file for %s-%s: %s' % (namespace, keyname, error))
        return None

def save(namespace='system', keyname='', value=None):
    """
    Saves any type of Python primitive object into a json file.
    :param namespace:
    :param keyname:
    :param value:
    :return:
    """
    import ujson
    if namespace == '' or keyname is None:
        print('valuestore.save() called with empty namespace')
        return None
    if keyname == '' or keyname is None:
        print('valuestore.save() called with empty keyname')
        return None
    if value == None:
        print('valuestore.save() called with empty value')
        return None

    try:
        with open('/config/%s-%s.json' % (namespace, keyname), 'w') as file:
            return ujson.dump(value, file)
    except BaseException as error:
        print('Error reading cache file for %s-%s: %s' % (namespace, keyname, error))
        return None

def stat(namespace='system', keyname=''):
    """
    Returns the file status as reported by os.stat (a 9-value tuple).
    :param namespace:
    :param keyname:
    :return:
    """
    import os
    if namespace == '' or keyname is None:
        print('valuestore.load() called with empty namespace')
        return None
    if keyname == '' or keyname is None:
        print('valuestore.load() called with empty keyname')
        return None

    try:
        return os.stat('/config/%s-%s.json' % (namespace, keyname))
    except BaseException as error:
        return (-1, -1, -1, -1, -1, -1, -1, -1, -1, -1)

def last_modified(namespace='system', keyname=''):
    """
    Returns when the given store item was last updated.
    :param namespace:
    :param keyname:
    :return:
    """
    stat_data = stat(namespace, keyname)
    if not stat_data:
        return None
    else:
        return stat_data[8]  # Last modified information