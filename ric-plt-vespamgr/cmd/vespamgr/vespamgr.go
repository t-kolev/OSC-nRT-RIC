/*
 *  Copyright (c) 2020 AT&T Intellectual Property.
 *  Copyright (c) 2020 Nokiv.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * This source code is part of the near-RT RIC (RAN Intelligent Controller)
 * platform project (RICP).
 */

package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"strings"
	"time"

	app "gerrit.o-ran-sc.org/r/ric-plt/xapp-frame/pkg/xapp"
)

func NewVespaMgr() *VespaMgr {
	return &VespaMgr{
		rmrReady:             false,
		chVesagent:           make(chan error),
		appmgrHost:           app.Config.GetString("controls.appManager.host"),
		appmgrUrl:            app.Config.GetString("controls.appManager.path"),
		appmgrNotifUrl:       app.Config.GetString("controls.appManager.notificationUrl"),
		appmgrSubsUrl:        app.Config.GetString("controls.appManager.subscriptionUrl"),
		appmgrRetry:          app.Config.GetInt("controls.appManager.appmgrRetry"),
		hbInterval:           app.Config.GetString("controls.vesagent.hbInterval"),
		measInterval:         app.Config.GetString("controls.vesagent.measInterval"),
		prometheusAddr:       app.Config.GetString("controls.vesagent.prometheusAddr"),
		alertManagerBindAddr: app.Config.GetString("controls.vesagent.alertManagerBindAddr"),
	}
}

func (v *VespaMgr) Run(sdlcheck, runXapp bool) {
	app.Logger.SetMdc("vespamgr", fmt.Sprintf("%s:%s", Version, Hash))
	app.SetReadyCB(func(d interface{}) { v.rmrReady = true }, true)
	app.Resource.InjectStatusCb(v.StatusCB)
	app.AddConfigChangeListener(v.ConfigChangeCB)

	measUrl := app.Config.GetString("controls.measurementUrl")
	app.Resource.InjectRoute(v.appmgrNotifUrl, v.HandlexAppNotification, "POST")
	app.Resource.InjectRoute(measUrl, v.HandleMeasurements, "POST")
	app.Resource.InjectRoute("/supervision", v.HandleSupervision, "GET") // @todo: remove this
	app.Resource.InjectRoute("/ric/v1/symptomdata", v.SymptomDataHandler, "GET")

	go v.SubscribeXappNotif(fmt.Sprintf("%s%s", v.appmgrHost, v.appmgrSubsUrl))

	if runXapp {
		app.RunWithParams(v, sdlcheck)
	}
}

func (v *VespaMgr) SymptomDataHandler(w http.ResponseWriter, r *http.Request) {
	appConfig, err := ioutil.ReadFile(app.Config.GetString("controls.vesagent.configFile"))
	if err != nil {
		app.Logger.Error("Unable to read config file: %v", err)
	}
	app.Logger.Info("SymptomDataHandler: appConfig=%+v", string(appConfig))

	baseDir := app.Resource.CollectDefaultSymptomData("app-config.json", appConfig)
	if baseDir != "" {
		app.Resource.SendSymptomDataFile(w, r, baseDir, "symptomdata.zip")
	}
}

func (v *VespaMgr) Consume(rp *app.RMRParams) (err error) {
	app.Logger.Info("Message received!")

	app.Rmr.Free(rp.Mbuf)
	return nil
}

func (v *VespaMgr) StatusCB() bool {
	if !v.rmrReady {
		app.Logger.Info("RMR not ready yet!")
	}

	return v.rmrReady
}

func (v *VespaMgr) ConfigChangeCB(configparam string) {
	return
}

func (v *VespaMgr) CreateConf(fname string, xappMetrics []byte) {
	f, err := os.Create(fname)
	if err != nil {
		app.Logger.Error("os.Create failed: %s", err.Error())
		return
	}
	defer f.Close()

	v.CreateConfig(f, xappMetrics)
}

func (v *VespaMgr) QueryXappConf(appmgrUrl string) (appConfig []byte, err error) {
	client := http.Client{Timeout: 10 * time.Second}

	for i := 0; i < v.appmgrRetry; i++ {
		app.Logger.Info("Getting xApp config from: %s [%d]", appmgrUrl, v.appmgrRetry)

		resp, err := client.Get(appmgrUrl)
		if err != nil || resp == nil {
			app.Logger.Error("client.Get failed: %v", err)
			time.Sleep(5 * time.Second)
			continue
		}

		defer resp.Body.Close()
		appConfig, err := ioutil.ReadAll(resp.Body)
		if err != nil {
			app.Logger.Error("ioutil.ReadAll failed: %v", err)
			time.Sleep(5 * time.Second)
			continue
		}

		app.Logger.Info("Received xApp config: %d", len(appConfig))
		if len(appConfig) > 0 {
			return appConfig, err
		}
	}

	return appConfig, err
}

func (v *VespaMgr) ReadPayload(w http.ResponseWriter, r *http.Request) ([]byte, error) {
	payload, err := ioutil.ReadAll(r.Body)
	defer r.Body.Close()
	if err != nil {
		app.Logger.Error("ioutil.ReadAll failed: %v", err)
		return payload, err
	}
	v.respondWithJSON(w, http.StatusOK, err)

	return payload, err
}

func (v *VespaMgr) HandleSupervision(w http.ResponseWriter, r *http.Request) {
	v.respondWithJSON(w, http.StatusOK, nil)
}

func (v *VespaMgr) HandleMeasurements(w http.ResponseWriter, r *http.Request) {
	if appConfig, err := v.ReadPayload(w, r); err == nil {
		filePath := app.Config.GetString("controls.pltFile")
		if err := ioutil.WriteFile(filePath, appConfig, 0666); err == nil {
			v.pltFileCreated = true
		}
	}
}

func (v *VespaMgr) HandlexAppNotification(w http.ResponseWriter, r *http.Request) {
	if _, err := v.ReadPayload(w, r); err != nil {
		return
	}

	app.Logger.Info("xApp event notification received!")
	if appConfig, err := v.QueryXappConf(fmt.Sprintf("%s%s", v.appmgrHost, v.appmgrUrl)); err == nil {
		v.CreateConf(app.Config.GetString("controls.vesagent.configFile"), appConfig)
		v.RestartVesagent()
	}
}

func (v *VespaMgr) DoSubscribe(appmgrUrl string, subscriptionData []byte) string {
	resp, err := http.Post(appmgrUrl, "application/json", bytes.NewBuffer(subscriptionData))
	if err != nil || resp == nil || resp.StatusCode != http.StatusCreated {
		app.Logger.Error("http.Post failed: %s", err)
		return ""
	}

	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		app.Logger.Error("ioutil.ReadAll for body failed: %s", err)
		return ""
	}

	var result map[string]interface{}
	if err := json.Unmarshal([]byte(body), &result); err != nil {
		app.Logger.Error("json.Unmarshal failed: %s", err)
		return ""
	}
	v.subscriptionId = result["id"].(string)
	app.Logger.Info("Subscription id from the response: %s", v.subscriptionId)

	return v.subscriptionId
}

func (v *VespaMgr) SubscribeXappNotif(appmgrUrl string) {
	targetUrl := fmt.Sprintf("%s%s", app.Config.GetString("controls.host"), v.appmgrNotifUrl)
	subscriptionData := []byte(fmt.Sprintf(`{"Data": {"maxRetries": 5, "retryTimer": 5, "eventType":"all", "targetUrl": "%v"}}`, targetUrl))

	for i := 0; i < v.appmgrRetry; i++ {
		app.Logger.Info("Subscribing xApp notification from: %v", appmgrUrl)
		if id := v.DoSubscribe(appmgrUrl, subscriptionData); id != "" {
			app.Logger.Info("Subscription done, id=%s", id)
			break
		}

		app.Logger.Info("Subscription failed, retyring after short delay ...")
		time.Sleep(5 * time.Second)
	}

	if xappConfig, err := v.QueryXappConf(fmt.Sprintf("%s%s", v.appmgrHost, v.appmgrUrl)); err == nil {
		v.CreateConf(app.Config.GetString("controls.vesagent.configFile"), xappConfig)
		v.RestartVesagent()
	}
}

func (v *VespaMgr) respondWithJSON(w http.ResponseWriter, code int, payload interface{}) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(code)
	if payload != nil {
		response, _ := json.Marshal(payload)
		w.Write(response)
	}
}

func (v *VespaMgr) StartVesagent() {
	v.vesAgent = NewCommandRunner("ves-agent", "-i", v.hbInterval, "-m", v.measInterval, "--Debug",
		"--Measurement.Prometheus.Address", v.prometheusAddr, "--AlertManager.Bind", v.alertManagerBindAddr)

	v.vesAgent.Run(v.chVesagent)
}

func (v *VespaMgr) RestartVesagent() {
	if strings.Contains(app.Config.GetString("controls.host"), "localhost") {
		return
	}

	if v.vesAgent != nil {
		err := v.vesAgent.Kill()
		if err != nil {
			app.Logger.Error("Couldn't kill vespa-agent: %s", err.Error())
			return
		}
		<-v.chVesagent
	}

	v.StartVesagent()
}

func main() {
	NewVespaMgr().Run(false, true)
}
