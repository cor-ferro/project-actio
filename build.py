import urllib.request
import json
import os
import subprocess
import glob
from shutil import copy, rmtree
from pathlib import Path

CURRENT_DIR = os.getcwd()
DEST_LIBS_DIR = CURRENT_DIR + "/libs/linux64"
DEST_BIN_DIR = CURRENT_DIR + "/bin/linux64"


def copy_to_libs(pattern):
	files = glob.glob(pattern)
	for file in files:
		print("copy %s => %s" % (file, DEST_LIBS_DIR))
		copy(file, DEST_LIBS_DIR)


def copy_to_bin(pattern):
	files = glob.glob(pattern)
	for file in files:
		print("copy %s => %s" % (file, DEST_BIN_DIR))
		copy(file, DEST_BIN_DIR)


class Boost:
	version = "1.65.1"

	@staticmethod
	def libs():
		return ["system", "thread", "timer", "chrono", "date_time", "filesystem", "regex"]

	def archive(self):
		return ''.join(["boost_", self.version.replace(".", "_"), ".zip"])

	def directory(self):
		return ''.join(["./boost_", self.version.replace(".", "_")])

	def build(self):
		print("Build boost", self.version)

		# "https://dl.bintray.com/boostorg/release/$version/source/$file_name.zip.json" -O $file_name.zip.json
		boost_archive = Path(self.archive())

		if not boost_archive.exists():
			self.download()
		else:
			if not self.validate():
				self.download()

		self.build_libs()

	def download(self):
		print("download %s" % (self.archive()))

		file_name = Path(self.archive())
		url = ''.join(["https://dl.bintray.com/boostorg/release/", self.version, "/source/", self.archive()])
		with urllib.request.urlopen(url) as response, open(file_name, 'wb') as out_file:
			data = response.read()
			out_file.write(data)

		print("finish download")

		if os.path.exists(self.directory()):
			rmtree(self.directory())

		self.try_unzip()

	def validate(self):
		url = ''.join(["https://dl.bintray.com/boostorg/release/",  self.version, "/source/", self.archive(), ".json"])
		response = urllib.request.urlopen(url)
		data = response.read()
		text = data.decode('utf-8')
		archive_json = json.loads(text)

		result = subprocess.check_output(["shasum", "-a", "256", "./" + self.archive()])
		result_hash = result.decode('utf-8')[0:64]

		return archive_json["sha256"] == result_hash

	def try_unzip(self):
		if not os.path.exists(self.directory()):
			print("unzip")
			subprocess.call(["unzip", self.archive()])
			print("unzip completed")

	def build_libs(self):
		print("build libs", self.libs())

		self.try_unzip()

		subprocess.call(["./bootstrap.sh", "--with-toolset=gcc", "--with-libraries=" + ','.join(self.libs())], cwd="./boost_1_65_1")
		subprocess.call(["./b2"], cwd="./boost_1_65_1")

		copy_to_libs("./boost_1_65_1/stage/lib/*.so*")
		copy_to_bin("./boost_1_65_1/stage/lib/*.a")


class Assimp:



lib = Boost()
lib.build()
