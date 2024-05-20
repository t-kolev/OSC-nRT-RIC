# RICDMS

- [RICDMS](#ricdms)
  - [Building](#building)
    - [Local build and Run](#local-build-and-run)
    - [Kubernetes](#kubernetes)
  - [Developer Environment](#developer-environment)
    - [Mocking all the dependent services (chartmuseum, appmgr...)](#mocking-all-the-dependent-services-chartmuseum-appmgr)
    - [Running the RICDMS](#running-the-ricdms)


## Building

### Local build and Run

Chekout code for [RICDMS](https://gerrit.o-ran-sc.org/r/admin/repos/ric-plt/ricdms) repository from gerrit.
```sh
$ git clone ssh://subhash_singh@gerrit.o-ran-sc.org:29418/ric-plt/ricdms
```

build locally
```sh
$ make build
```

Run the executable
```sh
$./ricdms
{"ts":1684321663015,"crit":"INFO","id":"ricdms","mdc":{},"msg":"Logger is initialized without config file()."}
{"ts":1684321663023,"crit":"INFO","id":"ricdms","mdc":{},"msg":"Starting server at : 0.0.0.0:8000"}
2023/05/17 11:07:43 Serving r i c d m s at http://[::]:8000
```

It will start the RICDMS on port `:8000`

### Kubernetes

Build the image
```
$ make image
```

Add the changes to `deployment/dms-config.yaml` as per your environment (refer your `.kubeconfig` file).
```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: kube-env
data:
  config: |
    apiVersion: v1
    clusters:
    - cluster:
        certificate-authority-data: <certificate>
        server: https://<kube-ip>:<kube-port>
      name: <name>
    contexts:
    - context:
        cluster:<cluster-name>
        user: <user>
      name: <name>
    current-context: <context>
    kind: Config
    preferences: {}
    users:
    - name: <name>
      user:
        client-certificate-data: <cliet-cert> 
        client-key-data: <client-key-data>
```

Apply the deployment yaml :
```
$ kubectl apply -f deployment
```

Make sure that following pod, svc and configmap is created :
```sh
$ kubectl get po,svc,config
NAME                                          READY   STATUS    RESTARTS   AGE
pod/dms-server-r2k64                          1/1     Running   0          15s

NAME                             TYPE        CLUSTER-IP      EXTERNAL-IP   PORT(S)          AGE
service/ric-dms-service          NodePort    <cluster-IP>    <none>        8000:32625/TCP   15s

NAME                         DATA   AGE
configmap/kube-env           1      90s
```

## Developer Environment

### Mocking all the dependent services (chartmuseum, appmgr...)
One can bring [`moco`](https://github.com/dreamhead/moco) server to mock environment for development purpose. Mocks are captured in `mock` folder.

Use below command to bring up the mock server :
```sh
$ docker run --rm -d -v /home/ubuntu/osc/ric-plt/ricdms/mock:/var/moco -p 9191:8000 rezzza/docker-moco:latest
```

### Running the RICDMS
Export the `` enviroment variable to use the URLs from `` so that mock server could be used.
```bash
$ export RIC_DMS_CONFIG_FILE=$(pwd)/config/config-test.yaml
```

Now, developer environment is ready :computer: