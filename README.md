# Paradigm Hyperloop

Comp III Control Software and Firmware. 

## Important Documentation

 - SpaceX Rules can be found [here](http://www.spacex.com/sites/spacex/files/2018_hyperloop_competition_rules.pdf)
 - Tube Specifications are continually being released. Check the Paradigm Hyperloop Google Drive for updated versions
 - Check the `Paradigm Hyperloop` > `04 Software` and `03 Electrical` folders for additional documentation, architecture specs, etc

## Firmware

Node board firmware is co-located in this repository to keep API implementation files consistent in the same git tree.

Firmware is currently developed using [platform.io](https://platform.io), but this is subject to change.

## Software

Software run on the BeageBone Black can be found in this repo as well.

## Testing

Testing is performed by a Jenkins build server automatically.  Should your PR fail to build please have a look at the console output
in the jenkins job for context clues. The Jenkins server executes the tasks listed in the `Jenkinsfile` and will test the Firmware and
Embedded Software in various ways.  You can reproduce those tests by following the steps in the `Jenkinsfile`.

# Contact

Please feel free to get in touch with us via [our contact form](https://paradigmhyperloop.com/contact/)

# People

Please check [our website](https://paradigmhyperloop.com) for our roster. Some of our members are also listed [here on Github](https://github.com/orgs/ParadigmHyperloop/people).

# License

See the [LICENSE](LICENSE) file found at the root of this repository and in the headers of our source code.