### Amazon FreeRTOS Qualifcation Program (AFQP) Check

#### Dependencies
* Python 2.7+ or Python 3+

#### Usage
The default Amazon FreeRTOS source root is relative to tools/checks/afqp/afqp_check. This can be changed with the --root option.
* **Non-eclipse based IDE projects.**
    ```
    cd <AFR_ROOT>/tools/checks/afqp/afqp_check
    python src/afqp_check.py --vendor <vendor_folder_name> --board <board_folder_name> --ide <ide_folder_name>
    ```
* **Eclipse based IDE projects.**
    ```
    cd <AFR_ROOT>/tools/checks/afqp/afqp_check
    python src/afqp_check.py --vendor <vendor_folder_name> --board <board_folder_name> --ide <ide_folder_name>  --eclipse
    ```
    If the eclipse project files are not located directly under <AFR_ROOT>/demos/vendor/board/ide, then you must specify the location relative to <AFR_ROOT>/demos/vendor/board/ide.
    ```
    cd <AFR_ROOT>/tools/checks/afqp/afqp_check
    python src/afqp_check.py --vendor <vendor_folder_name> --board <board_folder_name> --ide <ide_folder_name>  --eclipse --project aws_demos
    ```

#### The Checks
* Check for required AFQP files:
    * see **tools/checks/afqp/fs_checkrules.json**
* Check for **configPLATFORM_NAME** in **FreeRTOSConfig.h**
    * Return *error* if missing.
* Check for build artifiacts.
    * Return *error* if present.
* Check for vendor license in AFQP files.
    * Return *warning* if the Amazon FreeRTOS license is in ported code.
    * Return *error* for each missing Amazon FreeRTOS copyright.
    * Return *error* for incorrect copyright year.
    * Return *error* for incorrect versions on ported files.
* Check for a compliant Eclipse project.
    * Return *error* for naming demo project anything other than "**aws_demos**" in the **.project** file.
    * Return *error* for multiple project roots defined in the ide's **.project** file.
    * Return *error" for each missing **\<locationURI\>** tag in the linked resource in the **.project** file.
    * Return *warning* for each include path in the **.cproject** not extending from the single root variable defined in the **.project**.
    
#### Testing
Run **pytest** from this directory (tools/checks/afqp/afqp_check)

##### Dependencies
* ```pytest```
* ```pytest-cov```
