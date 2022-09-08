from phew import server

@server.route("/random", methods=["GET"])
def random_number(request):
  import random
  bits = int(request.query.get("bits", 0))
  return str(random.getrandbits(bits))

@server.catchall()
def catchall(request):
  return "Not found", 404

server.run()
