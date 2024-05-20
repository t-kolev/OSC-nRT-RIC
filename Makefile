#   Copyright (c) 2022 Samsung
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#

build: generate
	go build -o ricdms cmd/ric-dms.go

generate:
	swagger generate server -f api/ric-dms-api-2.0.yaml -t pkg/ --exclude-main

run:
	go run cmd/ric-dms.go

test:
	go clean -testcache
	go test -timeout 30s -run ^Test gerrit.o-ran-sc.org/r/ric-plt/ricdms/pkg/resthooks -v 

image:
	docker build -t  ric-dms:v1.0 .

docker-run:
	docker run -it --rm -d -p 8000:8000 ric-dms:v1.0

all: generate build test image
