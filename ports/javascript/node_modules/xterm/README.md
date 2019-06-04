# [![xterm.js logo](logo-full.png)](https://xtermjs.org)

[![Build Status](https://dev.azure.com/xtermjs/xterm.js/_apis/build/status/xtermjs.xterm.js)](https://dev.azure.com/xtermjs/xterm.js/_build/latest?definitionId=3)
[![Coverage Status](https://coveralls.io/repos/github/xtermjs/xterm.js/badge.svg?branch=master)](https://coveralls.io/github/xtermjs/xterm.js?branch=master)

Xterm.js is a front-end component written in TypeScript that lets applications bring fully-featured terminals to their users in the browser. It's used by popular projects such as VS Code, Hyper and Theia.

## Features

- **Terminal apps just work**: Xterm.js works with most terminal apps such as `bash`, `vim` and `tmux`, this includes support for curses-based apps and mouse event support.
- **Perfomant**: Xterm.js is *really* fast, it even includes a GPU-accelerated renderer.
- **Rich unicode support**: Supports CJK, emojis and IMEs.
- **Self-contained**: Requires zero dependencies to work.
- **Accessible**: Screen reader support can be turned on using the `screenReaderMode` option.
- **And much more**: Links, theming, addons, well documented API, etc.

## What xterm.js is not

- Xterm.js is not a terminal application that you can download and use on your computer.
- Xterm.js is not `bash`. Xterm.js can be connected to processes like `bash` and let you interact with them (provide input, receive output).

## Getting Started

First you need to install the module, we ship exclusively through [npm](https://www.npmjs.com/) so you need that installed and then add xterm.js as a dependency by running:

```
npm install xterm
```

To start using xterm.js on your browser, add the `xterm.js` and `xterm.css` to the head of your html page. Then create a `<div id="terminal"></div>` onto which xterm can attach itself. Finally instantiate the `Terminal` object and then call the `open` function with the DOM object of the `div`.

```html
<!doctype html>
  <html>
    <head>
      <link rel="stylesheet" href="node_modules/xterm/dist/xterm.css" />
      <script src="node_modules/xterm/dist/xterm.js"></script>
    </head>
    <body>
      <div id="terminal"></div>
      <script>
        var term = new Terminal();
        term.open(document.getElementById('terminal'));
        term.write('Hello from \x1B[1;3;31mxterm.js\x1B[0m $ ')
      </script>
    </body>
  </html>
```

### Importing

The recommended way to load xterm.js is via the ES6 module syntax:

```javascript
import { Terminal } from 'xterm';
```

### Addons

Addons are JavaScript modules that extend the `Terminal` prototype with new methods and attributes to provide additional functionality. There are a handful available in the main repository in the `src/addons` directory and you can even write your own by using the [public API](https://github.com/xtermjs/xterm.js/blob/master/typings/xterm.d.ts).

To use an addon, just import the JavaScript module and pass it to `Terminal`'s `applyAddon` method:

```javascript
import { Terminal } from 'xterm';
import * as fit from 'xterm/lib/addons/fit/fit';

Terminal.applyAddon(fit);

var xterm = new Terminal();  // Instantiate the terminal
xterm.fit();                 // Use the `fit` method, provided by the `fit` addon
```

You will also need to include the addon's CSS file if it has one in the folder.

#### Importing Addons in TypeScript

There are currently no typings for addons if they are accessed via extending Terminal prototype, so you will need to upcast if using TypeScript, eg. `(xterm as any).fit()`. Alternatively, you can import the addon function and enhance the terminal on demand. This has better typing support and is friendly to treeshaking.

```typescript
import { Terminal } from 'xterm';
import { fit } from 'xterm/lib/addons/fit/fit';
const xterm = new Terminal();

fit(xterm);  // Fit the terminal when necessary
```

## Browser Support

Since xterm.js is typically implemented as a developer tool, only modern browsers are supported officially. Here is a list of the versions we aim to support:

- Chrome latest
- Edge latest
- Firefox latest
- Safari latest
- IE11

Xterm.js works seamlessly in [Electron](https://electronjs.org/) apps and may even work on earlier versions of the browsers, these are the versions we strive to keep working.

## API

The full API for xterm.js is contained within the [TypeScript declaration file](https://github.com/xtermjs/xterm.js/blob/master/typings/xterm.d.ts), use the branch/tag picker in GitHub (`w`) to navigate to the correct version of the API.

Note that some APIs are marked *experimental*, these are added to enable experimentation with new ideas without committing to support it like a normal [semver](https://semver.org/) API. Note that these APIs can change radically between versions so be sure to read release notes if you plan on using experimental APIs.

## Real-world uses
Xterm.js is used in several world-class applications to provide great terminal experiences.

- [**SourceLair**](https://www.sourcelair.com/): In-browser IDE that provides its users with fully-featured Linux terminals based on xterm.js.
- [**Microsoft Visual Studio Code**](http://code.visualstudio.com/): Modern, versatile and powerful open source code editor that provides an integrated terminal based on xterm.js.
- [**ttyd**](https://github.com/tsl0922/ttyd): A command-line tool for sharing terminal over the web, with fully-featured terminal emulation based on xterm.js.
- [**Katacoda**](https://www.katacoda.com/): Katacoda is an Interactive Learning Platform for software developers, covering the latest Cloud Native technologies.
- [**Eclipse Che**](http://www.eclipse.org/che): Developer workspace server, cloud IDE, and Eclipse next-generation IDE.
- [**Codenvy**](http://www.codenvy.com): Cloud workspaces for development teams.
- [**CoderPad**](https://coderpad.io): Online interviewing platform for programmers. Run code in many programming languages, with results displayed by xterm.js.
- [**WebSSH2**](https://github.com/billchurch/WebSSH2): A web based SSH2 client using xterm.js, socket.io, and ssh2.
- [**Spyder Terminal**](https://github.com/spyder-ide/spyder-terminal): A full fledged system terminal embedded on Spyder IDE.
- [**Cloud Commander**](https://cloudcmd.io "Cloud Commander"): Orthodox web file manager with console and editor.
- [**Codevolve**](https://www.codevolve.com "Codevolve"): Online platform for interactive coding and web development courses. Live container-backed terminal uses xterm.js.
- [**RStudio**](https://www.rstudio.com/products/RStudio "RStudio"): RStudio is an integrated development environment (IDE) for R.
- [**Terminal for Atom**](https://github.com/jsmecham/atom-terminal-tab): A simple terminal for the Atom text editor.
- [**Eclipse Orion**](https://orionhub.org): A modern, open source software development environment that runs in the cloud. Code, deploy and run in the cloud.
- [**Gravitational Teleport**](https://github.com/gravitational/teleport): Gravitational Teleport is a modern SSH server for remotely accessing clusters of Linux servers via SSH or HTTPS.
- [**Hexlet**](https://en.hexlet.io): Practical programming courses (JavaScript, PHP, Unix, databases, functional programming). A steady path from the first line of code to the first job.
- [**Selenoid UI**](https://github.com/aerokube/selenoid-ui): Simple UI for the scallable golang implementation of Selenium Hub named Selenoid. We use XTerm for streaming logs over websockets from docker containers.
- [**Portainer**](https://portainer.io): Simple management UI for Docker.
- [**SSHy**](https://github.com/stuicey/SSHy): HTML5 Based SSHv2 Web Client with E2E encryption utilising xterm.js, SJCL & websockets.
- [**JupyterLab**](https://github.com/jupyterlab/jupyterlab): An extensible computational environment for Jupyter, supporting interactive data science and scientific computing across all programming languages.
- [**Theia**](https://github.com/theia-ide/theia): Theia is a cloud & desktop IDE framework implemented in TypeScript.
- [**Opshell**](https://github.com/ricktbaker/opshell) Ops Helper tool to make life easier working with AWS instances across multiple organizations.
- [**Proxmox VE**](https://www.proxmox.com/en/proxmox-ve): Proxmox VE is a complete open-source platform for enterprise virtualization. It uses xterm.js for container terminals and the host shell.
- [**Script Runner**](https://github.com/ioquatix/script-runner): Run scripts (or a shell) in Atom.
- [**Whack Whack Terminal**](https://github.com/Microsoft/WhackWhackTerminal): Terminal emulator for Visual Studio 2017.
- [**VTerm**](https://github.com/vterm/vterm): Extensible terminal emulator based on Electron and React.
- [**electerm**](http://electerm.html5beta.com): electerm is a terminal/ssh/sftp client(mac, win, linux) based on electron/node-pty/xterm.
- [**Kubebox**](https://github.com/astefanutti/kubebox): Terminal console for Kubernetes clusters.
- [**Azure Cloud Shell**](https://shell.azure.com): Azure Cloud Shell is a Microsoft-managed admin machine built on Azure, for Azure.
- [**atom-xterm**](https://atom.io/packages/atom-xterm): Atom plugin for providing terminals inside your Atom workspace.
- [**rtty**](https://github.com/zhaojh329/rtty): Access your terminals from anywhere via the web.
- [**Pisth**](https://github.com/ColdGrub1384/Pisth): An SFTP and SSH client for iOS.
- [**abstruse**](https://github.com/bleenco/abstruse): Abstruse CI is a continuous integration platform based on Node.JS and Docker.
- [**Azure Data Studio**](https://github.com/Microsoft/azuredatastudio): A data management tool that enables working with SQL Server, Azure SQL DB and SQL DW from Windows, macOS and Linux.
- [**FreeMAN**](https://github.com/matthew-matvei/freeman): A free, cross-platform file manager for power users.
- [**Fluent Terminal**](https://github.com/felixse/FluentTerminal): A terminal emulator based on UWP and web technologies.
- [**Hyper**](https://hyper.is): A terminal built on web technologies.
- [**Diag**](https://diag.ai): A better way to troubleshoot problems faster. Capture, share and reapply troubleshooting knowledge so you can focus on solving problems that matter.
- [**GoTTY**](https://github.com/yudai/gotty): A simple command line tool that shares your terminal as a web application based on xterm.js.
- [**genact**](https://github.com/svenstaro/genact): A nonsense activity generator.
- [**cPanel & WHM**](https://cpanel.com): The hosting platform of choice.
- [**Nutanix**](https://github.com/nutanix): Nutanix Enterprise Cloud uses xterm in the webssh functionality within Nutanix Calm, and is also looking to move our old noserial (termjs) functionality to xterm.js.
- [**SSH Web Client**](https://github.com/roke22/PHP-SSH2-Web-Client): SSH Web Client with PHP.
- [**Shellvault**](https://www.shellvault.io): The cloud-based SSH terminal you can access from anywhere.
- [**Juno**](http://junolab.org/): A flexible Julia IDE, based on Atom.
- [**webssh**](https://github.com/huashengdun/webssh): Web based ssh client.
- [**info-beamer hosted**](https://info-beamer.com): Uses xterm.js to manage digital signage devices from the web dashboard.
- [**Jumpserver**](https://github.com/jumpserver/luna): Jumpserver Luna project, Jumpserver is a bastion server project, Luna use xterm.js for web terminal emulation.
- [**LxdMosaic**](https://github.com/turtle0x1/LxdMosaic): Uses xterm.js to give terminal access to containers through LXD
- [**CodeInterview.io**](https://codeinterview.io): A coding interview platform in 25+ languages and many web frameworks. Uses xterm.js to provide shell access.
- [**Bastillion**](https://www.bastillion.io): Bastillion is an open-source web-based SSH console that centrally manages administrative access to systems.
- [**PHP App Server**](https://github.com/cubiclesoft/php-app-server/): Create lightweight, installable almost-native applications for desktop OSes.  ExecTerminal (nicely wraps the xterm.js Terminal), TerminalManager, and RunProcessSDK are self-contained, reusable ES5+ compliant Javascript components.

[And much more...](https://github.com/xtermjs/xterm.js/network/dependents)

Do you use xterm.js in your application as well? Please [open a Pull Request](https://github.com/sourcelair/xterm.js/pulls) to include it here. We would love to have it in our list. Note: Please add any new contributions to the end of the list only.

## Releases

Xterm.js follows a monthly release cycle roughly.

All current and past releases are available on this repo's [Releases page](https://github.com/sourcelair/xterm.js/releases), you can view the [high-level roadmap on the wiki](https://github.com/xtermjs/xterm.js/wiki/Roadmap) and see what we're working on now by looking through [Milestones](https://github.com/sourcelair/xterm.js/milestones).

### Beta builds

Our CI releases beta builds to npm for every change that goes into master, install the latest beta build with:

```
npm install -S xterm@beta
```

These should generally be stable but some bugs may slip in, we recommend using the beta build primarily to test out new features and for verifying bug fixes.

## Contributing

You can read the [guide on the wiki](https://github.com/xtermjs/xterm.js/wiki/Contributing) to learn how to contribute and setup xterm.js for development.

## License Agreement

If you contribute code to this project, you are implicitly allowing your code to be distributed under the MIT license. You are also implicitly verifying that all code is your original work.

Copyright (c) 2017-2019, [The xterm.js authors](https://github.com/xtermjs/xterm.js/graphs/contributors) (MIT License)<br>
Copyright (c) 2014-2017, SourceLair, Private Company ([www.sourcelair.com](https://www.sourcelair.com/home)) (MIT License)<br>
Copyright (c) 2012-2013, Christopher Jeffrey (MIT License)
