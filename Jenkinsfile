pipeline {
  agent any
  stages {
    stage('Build') {
      steps {
        parallel(
          "Build": {
            sh 'cmake -DCMAKE_C_COMPILER=/usr/bin/clang .'
            sh 'make -d clean'
            sh 'make -d all'
            sh 'mkdir -p ./BUILD/dst'
            sh 'make -d DESTDIR=./BUILD/dst install'
          }
        )
      }
    }
    stage('Test') {
      steps {
        sh 'make integration'
      }
    }
    stage('Build-32bit') {
      steps {
        sh 'cmake -DCMAKE_C_COMPILER=/usr/bin/clang .'
        sh 'make -d clean'
        sh 'make -d CFLAGS=-m32 all'
        sh 'mkdir -p ./BUILD/dst'
        sh 'make -d DESTDIR=./BUILD/dst install'
      }
    }
    stage('Test-32') {
      steps {
        sh 'make integration'
      }
    }
  }
}
