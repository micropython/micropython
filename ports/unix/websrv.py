from phew import server

@server.route("/simple", methods=["GET"])
def simple_request(request):
    results = "Some result from API"
    otherRes = ["fruit", "apple", "orange"]
    modded = map(lambda a : f"<ul>{a}</ul>", otherRes)
    otherResStr = ' '.join(modded)
    ret = f"""
    <html>
        <head></head>
        <body>
            <p>This is something {results} in a web page</p> 
            <ul>
                {otherResStr}
            </ul>
        </body>
    </html>
    """
    return ret

@server.route("/get_simple", methods=["GET"])
def simple_request(request):
    import http_lib
    res = http_lib.http_get('https://catfact.ninja/fact')
    return res

@server.catchall()
def catchall(request):
    return "Not found", 404

server.run()
