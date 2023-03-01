import logging
from networking import AccessPoint, WifiScanner, WebServer
from common import write_config

def initial_configuration():
    # Setup Wifi Scanner
    scanner = WifiScanner()

    # Act as an access point
    ap = AccessPoint()
    ap.start()
    logging.logger.info("AP IP: %s", ap.get_ip())
    
    # Serve configuration page (blocking)
    configpage = ConfigWebPage()
    configpage.render_page(scanner.scan())
    ssid, password = configpage.wait_for_ssid_password()

    # Return new configuration
    write_config({"ssid": ssid, "password": password})


class ConfigWebPage(WebServer):
    def __init__(self):
        super().__init__()

    def render_page(self, ssids):
        html = """
        <table>
        <tr>
            <th>SSIDs</th>
        </tr>
        """
        for ssid in ssids:
            html += """
            <tr>
                <td>{}</td>
            </tr>
            """.format(ssid)
        html += """
        </table>
        
        </br>
        </br>

        <form>
            <label for="ssid">SSID:</label><br>
            <input type="text" id="ssid" name="ssid"><br>
            <label for="password">Password:</label><br>
            <input type="password" id="password" name="password"><br>
            <br>
            <input type="submit" value="Submit">
        </form>
        """
        self.set_html(html)

    def wait_for_ssid_password(self):
        response = super().wait_for_response()
        for r in response:
            if "ssid" in r:
                ssid = r.split("ssid=")[-1]
                ssid = ssid.split(" ")[0]
                ssid = ssid.replace("+", " ")
                ssid = ssid.replace("%27", "'")
            elif "password" in r:
                password = r.split("password=")[-1]
                password = password.split(" ")[0]
                password = password.replace("+", " ")
                password = password.replace("%27", "'")
        return ssid, password
