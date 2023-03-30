from micropython import const
from ustruct import unpack_from


class DriveMode:
	Idle = const(0)
	Manual = const(1)
	Balls = const(2)
	Line = const(3)
	Marker = const(4)
	QRScan = const(5)
	SimpleProgramming = const(6)
	Dance = const(7)


class Point:
	def __init__(self, x: int, y: int):
		self.x = x
		self.y = y


class Ball:
	def __init__(self, pos: Point, r: int, hue: int):
		self.pos = pos
		self.r = r
		self.hue = hue


class ArucoMarker:
	def __init__(self, projected: [Point], index: int):
		self.projected = projected
		self.id = index


class QRMarker:
	DATA_SIZE = const(53)

	def __init__(self, projected: [Point], data: bytearray):
		self.projected = projected
		self.data = data


class CamFrame:
	def __init__(self, size: int = 0, data: bytes = None):
		self.size = size
		self.data = data


class DriveInfo:
	def __init__(self, mode: DriveMode, frame: CamFrame, balls=None, qr_markers=None, aruco_markers=None):
		if aruco_markers is None:
			aruco_markers = []

		if qr_markers is None:
			qr_markers = []

		if balls is None:
			balls = []

		self.mode = mode
		self.frame = frame
		self.balls = balls
		self.qr_markers = qr_markers
		self.aruco_markers = aruco_markers

	@staticmethod
	def deserialize(buf: bytearray):
		offset = 0

		# Deserialize DriveMode
		mode, = unpack_from('B', buf, offset)
		offset += 1

		# get rid of MotorInfo data
		offset += 4

		# Deserialize CamFrame
		frame_size, = unpack_from('I', buf, offset)
		offset += 4

		if offset + frame_size > len(buf):
			raise ValueError("Deserialize data too short, lacks JPG frame")

		frame_data = buf[offset:offset + frame_size]
		offset += frame_size
		frame = CamFrame(frame_size, frame_data)

		balls = []
		aruco_markers = []
		qr_markers = []

		if mode == DriveMode.Balls:
			balls, offset = DriveInfo.deserialize_ball(buf, offset)
		elif mode == DriveMode.Marker:
			aruco_markers, offset = DriveInfo.deserialize_aruco(buf, offset)

			# get rid of MarkerAction
			offset += 1
		elif mode == DriveMode.QRScan:
			qr_markers, offset = DriveInfo.deserialize_qr(buf, offset)
			aruco_markers, offset = DriveInfo.deserialize_aruco(buf, offset)

		return DriveInfo(mode, frame, balls, qr_markers, aruco_markers)

	@staticmethod
	def deserialize_ball(buf: bytearray, offset: int):
		count, = unpack_from('B', buf, offset)
		offset += 1

		balls = []
		for _ in range(count):
			ball_data = unpack_from('hhHb', buf, offset)
			offset += 8

			pos = Point(ball_data[0], ball_data[1])
			r, hue = ball_data[2], ball_data[3]

			balls.append(Ball(pos, r, hue))

		return balls, offset

	@staticmethod
	def deserialize_aruco(buf: bytearray, offset: int):
		count, = unpack_from('B', buf, offset)
		offset += 1

		markers = []
		for _ in range(count):
			marker_data = unpack_from('hhhhhhhhH', buf, offset)
			offset += 18

			projected = [
				Point(marker_data[0], marker_data[1]),
				Point(marker_data[2], marker_data[3]),
				Point(marker_data[4], marker_data[5]),
				Point(marker_data[6], marker_data[7]),
			]
			index = marker_data[8]

			markers.append(ArucoMarker(projected, index))

		return markers, offset

	@staticmethod
	def deserialize_qr(buf: bytearray, offset: int):
		count, = unpack_from('B', buf, offset)
		offset += 1

		markers = []
		for _ in range(count):
			marker_data = unpack_from('hhhhhhhhH', buf, offset)
			offset += 16

			projected = [
				Point(marker_data[0], marker_data[1]),
				Point(marker_data[2], marker_data[3]),
				Point(marker_data[4], marker_data[5]),
				Point(marker_data[6], marker_data[7]),
			]

			data = buf[offset:offset + QRMarker.DATA_SIZE]
			offset += QRMarker.DATA_SIZE

			markers.append(QRMarker(projected, data))

		return markers, offset
