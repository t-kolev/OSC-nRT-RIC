/*
==================================================================================
  Copyright (c) 2022 Samsung

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   This source code is part of the near-RT RIC (RAN Intelligent Controller)
   platform project (RICP).
==================================================================================
*/

package policy

import (
	"encoding/json"
	"errors"
	"fmt"
	"strconv"
	"strings"

	"gerrit.o-ran-sc.org/r/ric-plt/a1/pkg/a1"
	"gerrit.o-ran-sc.org/r/ric-plt/a1/pkg/models"
	"gerrit.o-ran-sc.org/r/ric-plt/a1/pkg/notification"
	"gerrit.o-ran-sc.org/r/ric-plt/a1/pkg/restapi/operations/a1_mediator"
	"gerrit.o-ran-sc.org/r/ric-plt/sdlgo"
)

var policyTypeNotFoundError = errors.New("Policy Type Not Found")
var policyInstanceNotFoundError = errors.New("Policy Instance Not Found")

const (
	a1HandlerPrefix                 = "a1.policy_handler."
	a1PolicyPrefix                  = "a1.policy_type."
	a1MediatorNs                    = "A1m_ns"
	a1InstancePrefix                = "a1.policy_instance."
	a1NotificationDestinationPrefix = "a1.policy_notification_destination."
)

func NewPolicyManager(sdl *sdlgo.SyncStorage) *PolicyManager {
	return createPolicyManager(sdl)
}

func createPolicyManager(sdlInst iSdl) *PolicyManager {
	pm := &PolicyManager{
		db: sdlInst,
	}
	return pm
}
func (pm *PolicyManager) SetPolicyInstanceStatus(policyTypeId int, policyInstanceID string, status string) error {
	a1.Logger.Debug("In SetPolicyInstanceStatus message recieved for %d and %s", policyTypeId, policyInstanceID)
	instancehandlerKey := a1HandlerPrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + policyInstanceID
	err := pm.db.Set(a1MediatorNs, instancehandlerKey, status)
	if err != nil {
		a1.Logger.Error("error1 :%+v", err)
		return err
	}
	return nil
}

func (pm *PolicyManager) GetPolicyInstanceStatus(policyTypeId int, policyInstanceID string) (bool, error) {
	a1.Logger.Debug("In GetPolicyInstanceStatus message recieved for %d and %s", policyTypeId, policyInstanceID)
	instancehandlerKey := a1HandlerPrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + policyInstanceID
	keys := []string{instancehandlerKey}
	resp, err := pm.db.Get(a1MediatorNs, keys)
	if err != nil {
		a1.Logger.Error("error1 :%+v", err)
		return false, err
	}
	for _, key := range resp {
		if key == "OK" {
			return true, nil
		}
	}
	return false, nil
}

func (pm *PolicyManager) SendPolicyStatusNotification(policyTypeId int, policyInstanceID string, handler string, status string) error {
	a1.Logger.Debug("In SendPolicyStatusNotification status message recieved for %d and %s", policyTypeId, policyInstanceID)
	notificationDestinationkey := a1NotificationDestinationPrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + fmt.Sprint(policyInstanceID)
	keys := [1]string{notificationDestinationkey}
	data, err := pm.db.Get(a1MediatorNs, keys[:])
	if err != nil {
		a1.Logger.Error("error1 :%+v", err)
		return err
	}

	if data[notificationDestinationkey] == nil {
		// notificationDestination URL is not available. Not an error, non-RT RIC
		// possibly not expecting any callback.
		return nil
	}

	notificationDestination, ok := data[notificationDestinationkey].(string)
	if !ok {
		return errors.New("failed to process notificationDestination URL")
	}

	policyInstanceStatus := a1_mediator.A1ControllerGetPolicyInstanceStatusOKBody{EnforceStatus: "ENFORCED"}
	enforced, err := pm.GetPolicyInstanceStatus(policyTypeId, policyInstanceID)
	if err != nil {
		return err
	}

	if !enforced {
		policyInstanceStatus.EnforceStatus = "NOT ENFORCED"
		policyInstanceStatus.EnforceReason = "OTHER_REASON"
	}

	jsonbody, err := json.Marshal(policyInstanceStatus)
	if err != nil {
		return err
	}
	err = notification.SendNotification(notificationDestination, string(jsonbody))
	if err != nil {
		return err
	}
	return nil
}

func (im *PolicyManager) GetAllPolicyInstance(policyTypeId int) ([]models.PolicyInstanceID, error) {
	a1.Logger.Debug("GetAllPolicyInstance")
	var policyTypeInstances = []models.PolicyInstanceID{}
	keys, err := im.db.GetAll("A1m_ns")

	if err != nil {
		a1.Logger.Error("error in retrieving policy. err: %v", err)
		return policyTypeInstances, err
	}
	a1.Logger.Debug("keys : %+v", keys)
	typekey := a1InstancePrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "."

	for _, key := range keys {
		if strings.HasPrefix(strings.TrimLeft(key, " "), typekey) {
			pti := strings.Split(strings.Trim(key, " "), typekey)[1]
			a1.Logger.Debug("pti %+v", pti)
			policyTypeInstances = append(policyTypeInstances, models.PolicyInstanceID(pti))
		}
	}

	if len(policyTypeInstances) == 0 {
		a1.Logger.Debug("policy instance Not Present  ")
		return policyTypeInstances, policyInstanceNotFoundError
	}

	a1.Logger.Debug("return : %+v", policyTypeInstances)
	return policyTypeInstances, nil
}

func (im *PolicyManager) GetPolicyInstance(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID) (interface{}, error) {
	a1.Logger.Debug("GetPolicyInstance1")

	var keys [1]string

	typekey := a1PolicyPrefix + strconv.FormatInt((int64(policyTypeId)), 10)
	keys[0] = typekey

	a1.Logger.Debug("key1 : %+v", typekey)

	valmap, err := im.db.Get(a1MediatorNs, keys[:])
	if len(valmap) == 0 {
		a1.Logger.Debug("policy type Not Present for policyid : %v", policyTypeId)
		return nil, policyTypeNotFoundError
	}

	if err != nil {
		a1.Logger.Error("error in retrieving policy type. err: %v", err)
		return nil, err
	}

	if valmap[typekey] == nil {
		a1.Logger.Debug("policy type Not Present for policyid : %v", policyTypeId)
		return nil, policyTypeNotFoundError
	}

	a1.Logger.Debug("keysmap : %+v", valmap[typekey])

	instancekey := a1InstancePrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + string(policyInstanceID)
	a1.Logger.Debug("key2 : %+v", instancekey)
	keys[0] = instancekey
	instanceMap, err := im.db.Get(a1MediatorNs, keys[:])
	if err != nil {
		a1.Logger.Error("policy instance error : %v", err)
		return nil, err
	}
	a1.Logger.Debug("policyinstancetype map : %+v", instanceMap)

	if instanceMap[instancekey] == nil {
		a1.Logger.Debug("policy instance Not Present for policyinstaneid : %v", policyInstanceID)
		return nil, policyInstanceNotFoundError
	}

	valStr := fmt.Sprint(instanceMap[instancekey])
	return valStr, nil
}
