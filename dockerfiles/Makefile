DOCKER := docker
DIRS := $(wildcard */)
TARGETS := $(addsuffix .build, $(DIRS))
export
UID := $(shell id -u)
GID := $(shell id -g)

build: $(TARGETS)

%.build: %/Dockerfile Makefile
	$(DOCKER) build \
		--build-arg UID=${UID} \
		--build-arg GID=${GID} \
		-t $(@D) \
		$(@D)
	$(DOCKER) inspect $(@D) \
		| jq -r '.[].RepoTags[]' > $@

clean: $(TARGETS)
	-cat $^ | xargs -I {} docker rmi -f {}
	-rm -f $^
