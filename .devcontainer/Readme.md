Build CircuitPython in a Github-Devcontainer
============================================

To build CircuitPython within a Github-Devcontainer, you need to perform
the following steps.

  1. checkout the code to a devcontainer

    - click on the green "<> Code"-button
    - select the Codespaces-tab
    - choose "+ new with options..." from the "..."-menu
    - in the following screen select the branch and then
    - select ".devcontainer/cortex-m/devcontainer.json" instead
      of "Default Codespaces configuration"
    - update region as necessary
    - finally, click on the green "Create codespace" button

  2. Your codespace is created. Cloning the images is quite fast, but
     preparing it for CircuitPython-development takes about 10 minutes.
     Note that this is a one-time task.

  3. During creation, you can run the command
     `tail -f /workspaces/.codespaces/.persistedshare/creation.log`
     to see what is going on.

  4. To actually build CircuitPython, run

         cd ports/raspberrypi
         make -j $(nproc) BOARD=whatever TRANSLATION=xx_XX

     This takes about 2m40s.
