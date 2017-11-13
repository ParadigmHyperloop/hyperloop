pipeline {
  agent any
  stages {
    stage('Build') {
      steps {
        parallel(
          "Build": {
            sh 'cmake -DCMAKE_C_COMPILER=/usr/bin/clang .'
            sh 'make VERBOSE=1 clean'
            sh 'make VERBOSE=1 all'
            sh 'mkdir -p ./BUILD/dst'
            sh 'make VERBOSE=1 DESTDIR=./BUILD/dst install'
          }
        )
      }
    }
    stage('Test') {
      steps {
        sh 'make integration'
        sh 'ctest'
      }
    }
    stage('Build-32bit') {
      steps {
        sh 'cmake -DCMAKE_C_COMPILER=/usr/bin/clang .'
        sh 'make VERBOSE=1 clean'
        sh 'make VERBOSE=1 CFLAGS=-m32 all'
        sh 'mkdir -p ./BUILD/dst'
        sh 'make VERBOSE=1 DESTDIR=./BUILD/dst install'
      }
    }
    stage('Test-32') {
      steps {
        sh 'make integration'
        sh 'ctest'
      }
    }
  }
}
