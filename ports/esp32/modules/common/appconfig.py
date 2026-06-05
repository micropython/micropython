import valuestore

def get(app_slug, default_config):
    config = valuestore.load(namespace='app', keyname=app_slug)
    resave = False

    if config is None:
        config = default_config
        resave = True

    for key in default_config.keys():
        if key not in config:
            config[key] = default_config[key]
            resave = True

    if resave:
        valuestore.save(namespace='app', keyname=app_slug, value=config)

    return config