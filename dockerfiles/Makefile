DOCKER := docker
DIRS := $(wildcard */)
TARGETS := $(addsuffix .build, $(DIRS))

build: $(TARGETS)

%.build:
	$(DOCKER) build \
		-t $(@D) \
		$(@D)
	$(DOCKER) inspect $(@D) \
		| jq -r '.[].RepoTags[]' > $@

clean: $(TARGETS)
	-cat $^ | xargs -I {} docker rmi -f {}
	-rm -f $^
