/*
==================================================================================
  Copyright (c) 2019 AT&T Intellectual Property.
  Copyright (c) 2019 Nokia

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
==================================================================================
*/

package nbi

import (
	"encoding/json"
	"fmt"
	"github.com/stretchr/testify/assert"
	"net"
	"net/http"
	"net/http/httptest"
	"os"
	"testing"
	"time"

	"errors"
	"gerrit.o-ran-sc.org/r/ric-plt/xapp-frame/pkg/xapp"
	apimodel "gerrit.oran-osc.org/r/ric-plt/o1mediator/pkg/appmgrmodel"
	"gerrit.oran-osc.org/r/ric-plt/o1mediator/pkg/sbi"
	"github.com/stretchr/testify/mock"
)

var XappConfig = `{
	"o-ran-sc-ric-ueec-config-v1:ric": {
	  "config": {
		"name": "ueec",
		"namespace": "ricxapp",
		"control": {
		  "active": true,
		  "interfaceId": {
			"globalENBId": {
			  "plmnId": "1234",
			  "eNBId": "55"
			}
		  }
		}
	  }
	}
  }`

var XappConfigErr = `{
       "ric": {
  }`

var XappDescriptor = `{
	"o-ran-sc-ric-xapp-desc-v1:ric": {
	  "xapps": {
		"xapp": [
		  {
			"name": "ueec",
			"release-name": "ueec-xapp",
			"version": "0.0.1",
			"namespace": "ricxapp"
		  }
		]
	  }
	}
  }`

var n *Nbi
var rnibM *rnibMock

// Test cases
func TestMain(M *testing.M) {
	rnibM = new(rnibMock)
	rnib = rnibM
	n = NewNbi(sbi.NewSBIClient("localhost:8080", "localhost:9093", 5))
	go n.Start()
	time.Sleep(time.Duration(1) * time.Second)

	os.Exit(M.Run())
}

func TestModifyConfigmap(t *testing.T) {
	ts := CreateHTTPServer(t, "PUT", "/ric/v1/config", 8080, http.StatusOK, apimodel.ConfigValidationErrors{})
	defer ts.Close()

	var f interface{}
	err := json.Unmarshal([]byte(XappConfig), &f)
	assert.Equal(t, true, err == nil)

	err = n.ManageConfigmaps("o-ran-sc-ric-ueec-config-v1", XappConfig, 1)
	assert.Equal(t, true, err == nil)
}

func TestXappDescModuleChangeCB(t *testing.T) {
	ok := n.testModuleChangeCB("o-ran-sc-ric-xapp-desc-v1")
	assert.True(t, ok)
}

func TestUeecConfigModuleChangeCB(t *testing.T) {
	ok := n.testModuleChangeCB("o-ran-sc-ric-ueec-config-v1")
	assert.True(t, ok)
}

func TestUeecConfigDoneModuleChangeCB(t *testing.T) {
	ok := n.testModuleChangeCBDone("o-ran-sc-ric-ueec-config-v1")
	assert.True(t, ok)
}

func TestXappDescGnbStateCB(t *testing.T) {
	ok := n.testGnbStateCB("o-ran-sc-ric-xapp-desc-v1")
	assert.True(t, ok)
}

func TestAlarmGnbStateCB(t *testing.T) {
	ok := n.testGnbStateCB("o-ran-sc-ric-alarm-v1")
	assert.True(t, ok)
}

func TestGnbStateCB(t *testing.T) {
	var rnibOk xapp.RNIBIRNibError
	var gNbIDs []*xapp.RNIBNbIdentity
	gNbID := xapp.RNIBNbIdentity{
		InventoryName: "test-gnb",
	}
	gNbIDs = append(gNbIDs, &gNbID)
	nodeInfo := xapp.RNIBNodebInfo{}

	rnibM.On("GetListGnbIds").Return(gNbIDs, rnibOk).Once()
	rnibM.On("GetNodeb", mock.Anything).Return(&nodeInfo, rnibOk).Once()
	ok := n.testGnbStateCB("")
	assert.True(t, ok)
}

func TestGnbStateCBWhenRnibGetListGnbIdsFails(t *testing.T) {
	var rnibErr xapp.RNIBIRNibError = errors.New("Some RNIB Error")

	rnibM.On("GetListGnbIds").Return(nil, rnibErr).Once()
	ok := n.testGnbStateCB("")
	assert.True(t, ok)
}

func TestGnbStateCBWhenRnibGetNodebFails(t *testing.T) {
	var rnibOk xapp.RNIBIRNibError
	var rnibErr xapp.RNIBIRNibError = errors.New("Some RNIB Error")
	var gNbIDs []*xapp.RNIBNbIdentity
	gNbID := xapp.RNIBNbIdentity{
		InventoryName: "test-gnb",
	}
	gNbIDs = append(gNbIDs, &gNbID)

	rnibM.On("GetListGnbIds").Return(gNbIDs, rnibOk).Once()
	rnibM.On("GetNodeb", mock.Anything).Return(nil, rnibErr).Once()
	ok := n.testGnbStateCB("")
	assert.True(t, ok)
}

func TestDeployXApp(t *testing.T) {
	ts := CreateHTTPServer(t, "POST", "/ric/v1/xapps", 8080, http.StatusCreated, apimodel.Xapp{})
	defer ts.Close()

	var f interface{}
	err := json.Unmarshal([]byte(XappDescriptor), &f)
	assert.Equal(t, true, err == nil)

	err = n.ManageXapps("o-ran-sc-ric-xapp-desc-v1", XappDescriptor, 0)
	assert.Equal(t, true, err == nil)
}

func TestUnDeployXApp(t *testing.T) {
	ts := CreateHTTPServer(t, "DELETE", "/ric/v1/xapps/ueec-xapp", 8080, http.StatusNoContent, apimodel.Xapp{})
	defer ts.Close()

	var f interface{}
	err := json.Unmarshal([]byte(XappDescriptor), &f)
	assert.Equal(t, true, err == nil)

	err = n.ManageXapps("o-ran-sc-ric-xapp-desc-v1", XappDescriptor, 2)
	assert.Equal(t, true, err == nil)
}

func TestGetDeployedXapps(t *testing.T) {
	ts := CreateHTTPServer(t, "GET", "/ric/v1/xapps", 8080, http.StatusOK, apimodel.AllDeployedXapps{})
	defer ts.Close()

	err := sbiClient.GetDeployedXapps()
	assert.Equal(t, true, err == nil)
}

func TestErrorCases(t *testing.T) {
	// Invalid config
	err := n.ManageXapps("o-ran-sc-ric-xapp-desc-v1", "", 2)
	assert.Equal(t, true, err == nil)

	// Invalid module
	err = n.ManageXapps("", "{}", 2)
	assert.Equal(t, true, err == nil)

	// Unexpected module
	err = n.ManageXapps("o-ran-sc-ric-ueec-config-v1", "{}", 2)
	assert.Equal(t, true, err == nil)

	// Invalid operation
	err = n.ManageXapps("o-ran-sc-ric-ueec-config-v1", XappDescriptor, 1)
	assert.Equal(t, true, err == nil)

	// Invalid config
	err = n.ManageConfigmaps("o-ran-sc-ric-ueec-config-v1", "", 1)
	assert.Equal(t, true, err == nil)

	// Invalid operation
	err = n.ManageConfigmaps("o-ran-sc-ric-ueec-config-v1", "{}", 0)
	assert.Equal(t, true, err != nil)

        //Invalid json 
        err = n.ManageConfigmaps("o-ran-sc-ric-ueec-config-v1", XappConfigErr, 1)
        assert.Equal(t, true, err != nil)

	// Invalid operation
	err = n.ManageXapps("o-ran-sc-ric-ueec-config-v1", XappDescriptor, 5)
	assert.Equal(t, true, err == nil)

        //Invalid json
        err = n.ManageXapps("o-ran-sc-ric-ueec-config-v1", XappConfigErr, 1)
        assert.Equal(t, true, err != nil)
}

func TestConnStatus2Str(t *testing.T) {
	assert.Equal(t, n.ConnStatus2Str(0), "not-specified")
	assert.Equal(t, n.ConnStatus2Str(1), "connected")
	assert.Equal(t, n.ConnStatus2Str(2), "disconnected")
	assert.Equal(t, n.ConnStatus2Str(3), "setup-failed")
	assert.Equal(t, n.ConnStatus2Str(4), "connecting")
	assert.Equal(t, n.ConnStatus2Str(5), "shutting-down")
	assert.Equal(t, n.ConnStatus2Str(6), "shutdown")
	assert.Equal(t, n.ConnStatus2Str(1234), "not-specified")
}

func TestE2APProt2Str(t *testing.T) {
	assert.Equal(t, n.E2APProt2Str(0), "not-specified")
	assert.Equal(t, n.E2APProt2Str(1), "x2-setup-request")
	assert.Equal(t, n.E2APProt2Str(2), "endc-x2-setup-request")
	assert.Equal(t, n.E2APProt2Str(1111), "not-specified")
}

func TestNodeType2Str(t *testing.T) {
	assert.Equal(t, n.NodeType2Str(0), "not-specified")
	assert.Equal(t, n.NodeType2Str(1), "enb")
	assert.Equal(t, n.NodeType2Str(2), "gnb")
	assert.Equal(t, n.NodeType2Str(1111), "not-specified")
}

func TestTeardown(t *testing.T) {
	n.Stop()
}

func CreateHTTPServer(t *testing.T, method, url string, port, status int, respData interface{}) *httptest.Server {
	l, err := net.Listen("tcp", fmt.Sprintf("localhost:%d", port))
	if err != nil {
		t.Error("Failed to create listener: " + err.Error())
	}
	ts := httptest.NewUnstartedServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		assert.Equal(t, r.Method, method)
		assert.Equal(t, r.URL.String(), url)
		w.Header().Add("Content-Type", "application/json")
		w.WriteHeader(status)
		b, _ := json.Marshal(respData)
		w.Write(b)
	}))
	ts.Listener.Close()
	ts.Listener = l

	ts.Start()

	return ts
}

func DescMatcher(result, expected *apimodel.XappDescriptor) bool {
	if *result.XappName == *expected.XappName && result.HelmVersion == expected.HelmVersion &&
		result.Namespace == expected.Namespace && result.ReleaseName == expected.ReleaseName {
		return true
	}
	return false
}

func ConfigMatcher(result, expected *apimodel.XAppConfig) bool {
	if *result.Metadata.XappName == *expected.Metadata.XappName &&
		*result.Metadata.Namespace == *expected.Metadata.Namespace {
		return true
	}
	return false
}

type rnibMock struct {
	mock.Mock
}

func (m *rnibMock) GetListGnbIds() ([]*xapp.RNIBNbIdentity, xapp.RNIBIRNibError) {
	a := m.Called()
	if a.Get(0) == nil {
		return nil, a.Error(1)
	}
	return a.Get(0).([]*xapp.RNIBNbIdentity), a.Error(1)
}

func (m *rnibMock) GetNodeb(invName string) (*xapp.RNIBNodebInfo, xapp.RNIBIRNibError) {
	a := m.Called(invName)
	if a.Get(0) == nil {
		return nil, a.Error(1)
	}
	return a.Get(0).(*xapp.RNIBNodebInfo), a.Error(1)
}

func (m *rnibMock) GetListEnbIds() ([]*xapp.RNIBNbIdentity, xapp.RNIBIRNibError) {
	return nil, nil
}