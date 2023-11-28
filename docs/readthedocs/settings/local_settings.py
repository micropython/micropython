import os

# Directory that the project lives in, aka ../..
SITE_ROOT = "/".join(os.path.dirname(__file__).split("/")[0:-2])

TEMPLATE_DIRS = (
    "%s/templates/"
    % SITE_ROOT,  # Your custom template directory, before the RTD one to override it.
    "%s/readthedocs/templates/" % SITE_ROOT,  # Default RTD template dir
)
