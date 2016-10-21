all:
	. .env/bin/activate && pip install -r requirements.txt

run: clean all
	. .env/bin/activate && ./server.py

test:
	./test

clean:
	rm -f server.out server.err test-data.txt *.csv

start-influx:
	influxd -config /usr/local/etc/influxdb.conf &

start-grafana:
	grafana-server --config=/usr/local/etc/grafana/grafana.ini --homepath /usr/local/share/grafana cfg:default.paths.logs=/usr/local/var/log/grafana cfg:default.paths.data=/usr/local/var/lib/grafana cfg:default.paths.plugins=/usr/local/var/lib/grafana/plugins &

setup-mac:
	# Install HomeBrew
	/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
	# Install influx
	brew install influxdb
	# Install influx
	brew install grafana
	# Install pip
	sudo easy_install pip
	# Install virtualenv
	sudo pip install virtualenv
	# setup the venv
	virtualenv .env
