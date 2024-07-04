#imports
import os

#commands
uic = "c:\\Qt\\6.7.0\\msvc2019_64\\bin\\uic.exe"
moc = "c:\\Qt\\6.7.0\\msvc2019_64\\bin\\moc.exe"
rcc = "c:\\Qt\\6.7.0\\msvc2019_64\\bin\\rcc.exe"

#uic files
for root, dirs, files in os.walk("ui"):
	for file in files:
		src_path = os.path.join(root, file)
		obj_path = src_path.replace(".ui", ".hpp")
		obj_path = obj_path.replace("ui\\", "build\\uic\\")
		os.makedirs(os.path.dirname(obj_path), exist_ok = True)
		if not os.path.isfile(obj_path) or os.path.getmtime(src_path) > os.path.getmtime(obj_path):
			os.system("%s %s > %s" % (uic, src_path, obj_path))

#moc files
for root, dirs, files in os.walk("inc"):
	for file in files:
		src_path = os.path.join(root, file)
		obj_path = src_path.replace(".hpp", ".cpp")
		obj_path = obj_path.replace("inc\\", "build\\moc\\")
		os.makedirs(os.path.dirname(obj_path), exist_ok = True)
		if not os.path.isfile(obj_path) or os.path.getmtime(src_path) > os.path.getmtime(obj_path):
			os.system("%s -f Tensegrity\\%s %s > %s" % (moc, src_path, src_path, obj_path))

#rsc files
for root, dirs, files in os.walk("rsc"):
	for file in files:
		if file.endswith(".qrc"):
			src_path = os.path.join(root, file)
			obj_path = src_path.replace(".qrc", ".cpp")
			obj_path = obj_path.replace("rsc\\", "build\\rsc\\")
			os.makedirs(os.path.dirname(obj_path), exist_ok = True)
			if not os.path.isfile(obj_path) or os.path.getmtime(src_path) > os.path.getmtime(obj_path):
				os.system("%s %s > %s" % (rcc, src_path, obj_path))