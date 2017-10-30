pipeline {
  agent any
  stages {
    stage('Build') {
      steps {
        parallel(
          "Build": {
            sh 'cmake .'
            sh 'make -d clean'
            sh 'make -d all'
            sh 'mkdir -p ./ROOT'
            sh 'make -d DESTDIR=./ROOT install'
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
        sh 'cmake .'
        sh 'make -d clean'
        sh 'make -d CFLAGS=-m32 all'
        sh 'mkdir -p ./ROOT'
        sh 'make -d DESTDIR=./ROOT install'
      }
    }
    stage('Test-32') {
      steps {
        sh 'make integration'
      }
    }
  }
}
