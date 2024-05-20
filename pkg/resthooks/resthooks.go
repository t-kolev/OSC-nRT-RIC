/*
==================================================================================

	Copyright (c) 2021 Samsung

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
package resthooks

import (
	"encoding/json"
	"errors"
	"fmt"
	"strconv"
	"strings"
	"time"

	"gerrit.o-ran-sc.org/r/ric-plt/a1/pkg/a1"
	"gerrit.o-ran-sc.org/r/ric-plt/a1/pkg/models"
	"gerrit.o-ran-sc.org/r/ric-plt/a1/pkg/policy"
	"gerrit.o-ran-sc.org/r/ric-plt/a1/pkg/restapi/operations/a1_mediator"
	"gerrit.o-ran-sc.org/r/ric-plt/a1/pkg/rmr"
	"gerrit.o-ran-sc.org/r/ric-plt/sdlgo"
	"github.com/santhosh-tekuri/jsonschema/v5"
	"gopkg.in/yaml.v2"
)

const (
	a1PolicyPrefix                  = "a1.policy_type."
	a1MediatorNs                    = "A1m_ns"
	a1InstancePrefix                = "a1.policy_instance."
	a1NotificationDestinationPrefix = "a1.policy_notification_destination."
	a1InstanceMetadataPrefix        = "a1.policy_inst_metadata."
	a1HandlerPrefix                 = "a1.policy_handler."
	a1PolicyRequest                 = 20010
	a1EIDataDelivery                = 20017
)

var typeAlreadyError = errors.New("Policy Type already exists")
var InstanceAlreadyError = errors.New("Policy Instance already exists")
var typeMismatchError = errors.New("Policytype Mismatch")
var invalidJsonSchema = errors.New("Invalid Json ")
var policyInstanceNotFoundError = errors.New("Policy Instance Not Found")
var policyTypeNotFoundError = errors.New("Policy Type Not Found")
var policyTypeCanNotBeDeletedError = errors.New("tried to delete a type that isn't empty")
var policyInstanceCanNotBeDeletedError = errors.New("tried to delete a Instance that isn't empty")

func (rh *Resthook) CanPolicyInstanceBeDeleted(err error) bool {
	return err == policyInstanceCanNotBeDeletedError
}

func (rh *Resthook) CanPolicyTypeBeDeleted(err error) bool {
	return err == policyTypeCanNotBeDeletedError
}

func (rh *Resthook) IsPolicyTypeNotFound(err error) bool {
	return err == policyTypeNotFoundError
}

func (rh *Resthook) IsPolicyInstanceNotFound(err error) bool {
	return err == policyInstanceNotFoundError
}

func (rh *Resthook) IsTypeAlready(err error) bool {
	return err == typeAlreadyError
}
func (rh *Resthook) IsInstanceAlready(err error) bool {
	return err == InstanceAlreadyError
}
func (rh *Resthook) IsTypeMismatch(err error) bool {
	return err == typeMismatchError
}

func (rh *Resthook) IsValidJson(err error) bool {
	return err == invalidJsonSchema
}
func NewResthook() *Resthook {
	sdl := sdlgo.NewSyncStorage()
	policyManager := policy.NewPolicyManager(sdl)
	return createResthook(sdl, rmr.NewRMRSender(policyManager))
}

func createResthook(sdlInst iSdl, rmrSenderInst rmr.IRmrSender) *Resthook {
	rh := &Resthook{
		db:             sdlInst,
		iRmrSenderInst: rmrSenderInst,
	}

	return rh
}

func (rh *Resthook) GetA1Health() bool {
	_, err := rh.db.GetAll("A1m_ns")
	if err != nil {
		a1.Logger.Error("error in connecting to the database. err: %v", err)
		return false
	}
	a1.Logger.Debug("A1 is healthy")
	return true
}

func (rh *Resthook) GetAllPolicyType() []models.PolicyTypeID {

	var policyTypeIDs []models.PolicyTypeID

	keys, err := rh.db.GetAll("A1m_ns")

	if err != nil {
		a1.Logger.Error("error in retrieving policy. err: %v", err)
		return policyTypeIDs
	}
	a1.Logger.Debug("keys : %+v", keys)

	for _, key := range keys {
		if strings.HasPrefix(strings.TrimLeft(key, " "), a1PolicyPrefix) {
			pti := strings.Split(strings.Trim(key, " "), a1PolicyPrefix)[1]
			ptii, _ := strconv.ParseInt(pti, 10, 64)
			policyTypeIDs = append(policyTypeIDs, models.PolicyTypeID(ptii))
		}
	}

	a1.Logger.Debug("return : %+v", policyTypeIDs)
	return policyTypeIDs
}

func (rh *Resthook) GetPolicyType(policyTypeId models.PolicyTypeID) (*models.PolicyTypeSchema, error) {
	a1.Logger.Debug("GetPolicyType1")

	var policytypeschema *models.PolicyTypeSchema
	var keys [1]string

	key := a1PolicyPrefix + strconv.FormatInt((int64(policyTypeId)), 10)
	keys[0] = key

	a1.Logger.Debug("key : %+v", key)

	valmap, err := rh.db.Get(a1MediatorNs, keys[:])

	a1.Logger.Debug("policytype map : %+v", valmap)

	if len(valmap) == 0 {
		a1.Logger.Error("policy type Not Present for policyid : %v", policyTypeId)
		return policytypeschema, policyTypeNotFoundError
	}

	if err != nil {
		a1.Logger.Error("error in retrieving policy type. err: %v", err)
		return nil,policyTypeNotFoundError
	}

	if valmap[key] == nil {
		a1.Logger.Error("policy type Not Present for policyid : %v", policyTypeId)
		return policytypeschema,policyTypeNotFoundError
	}

	a1.Logger.Debug("keysmap : %+v", valmap[key])

	var item models.PolicyTypeSchema
	valStr := fmt.Sprint(valmap[key])

	a1.Logger.Debug("Policy type for %+v :  %+v", key, valStr)
	valkey := "`" + valStr + "`"
	valToUnmarshall, err := strconv.Unquote(valkey)
	if err != nil {
		a1.Logger.Error("unquote error : %+v", err)
		return nil,policyTypeNotFoundError
	}

	a1.Logger.Debug("Policy type for %+v :  %+v", key, string(valToUnmarshall))

	errunm := json.Unmarshal([]byte(valToUnmarshall), &item)

	a1.Logger.Debug(" Unmarshalled json : %+v", (errunm))
	a1.Logger.Debug("Policy type Name :  %v", (item.Name))

	return &item,nil
}

func (rh *Resthook) CreatePolicyType(policyTypeId models.PolicyTypeID, httprequest models.PolicyTypeSchema) error {
	a1.Logger.Debug("CreatePolicyType function")
	if policyTypeId != models.PolicyTypeID(*httprequest.PolicyTypeID) {
		//error message
		a1.Logger.Debug("Policytype Mismatch")
		return typeMismatchError
	}
	key := a1PolicyPrefix + strconv.FormatInt((int64(policyTypeId)), 10)
	a1.Logger.Debug("key %+v ", key)
	if data, err := httprequest.MarshalBinary(); err == nil {
		a1.Logger.Debug("Marshaled String : %+v", string(data))
		success, err1 := rh.db.SetIfNotExists(a1MediatorNs, key, string(data))
		a1.Logger.Info("success:%+v", success)
		if err1 != nil {
			a1.Logger.Error("error :%+v", err1)
			return err1
		}
		if !success {
			a1.Logger.Debug("Policy type %+v already exist", policyTypeId)
			return typeAlreadyError
		}
	}
	return nil
}

func toStringKeys(val interface{}) (interface{}, error) {
	var err error
	switch val := val.(type) {
	case map[interface{}]interface{}:
		m := make(map[string]interface{})
		for k, v := range val {
			k, ok := k.(string)
			if !ok {
				return nil, errors.New("found non-string key")
			}
			m[k], err = toStringKeys(v)
			if err != nil {
				return nil, err
			}
		}
		return m, nil
	case []interface{}:
		var l = make([]interface{}, len(val))
		for i, v := range val {
			l[i], err = toStringKeys(v)
			if err != nil {
				return nil, err
			}
		}
		return l, nil
	default:
		return val, nil
	}
}

func validate(httpBodyString string, schemaString string) bool {
	var m interface{}
	err := yaml.Unmarshal([]byte(httpBodyString), &m)
	if err != nil {
		a1.Logger.Error("Unmarshal error : %+v", err)
	}
	m, err = toStringKeys(m)
	if err != nil {
		a1.Logger.Error("Conversion to string error : %+v", err)
		return false
	}
	compiler := jsonschema.NewCompiler()
	if err := compiler.AddResource("schema.json", strings.NewReader(schemaString)); err != nil {
		a1.Logger.Error("string reader error : %+v", err)
		return false
	}
	schema, err := compiler.Compile("schema.json")
	if err != nil {
		a1.Logger.Error("schema json compile error : %+v", err)
		return false
	}
	if err := schema.Validate(m); err != nil {
		a1.Logger.Error("schema validation error : %+v", err)
		return false
	}
	a1.Logger.Debug("validation successfull")
	return true
}

func (rh *Resthook) storePolicyInstance(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID, httpBody interface{}, notificationDestination string) (string, error) {
	var keys [1]string
	operation := "CREATE"
	typekey := a1PolicyPrefix + strconv.FormatInt((int64(policyTypeId)), 10)
	keys[0] = typekey

	a1.Logger.Debug("key1 : %+v", typekey)

	valmap, err := rh.db.Get(a1MediatorNs, keys[:])
	if err != nil {
		a1.Logger.Error("policy type error : %+v", err)
	}
	a1.Logger.Debug("policytype map : %+v", valmap)
	if valmap[typekey] == nil {
		a1.Logger.Error("policy type Not Present for policyid : %v", policyTypeId)
		return operation, policyTypeNotFoundError
	}
	// TODO : rmr creation_timestamp := time.Now() // will be needed for rmr to notify the creation of instance

	instancekey := a1InstancePrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + string(policyInstanceID)
	notificationDestinationkey := a1NotificationDestinationPrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + string(policyInstanceID)
	keys[0] = instancekey
	instanceMap, err := rh.db.Get(a1MediatorNs, keys[:])
	if err != nil {
		a1.Logger.Error("policy type error : %v", err)
	}
	a1.Logger.Debug("policyinstancetype map : %+v", instanceMap)

	if instanceMap[instancekey] != nil {
		operation = "UPDATE"
		a1.Logger.Debug("UPDATE")
		data, _ := json.Marshal(httpBody)
		a1.Logger.Debug("Marshaled String : %+v", string(data))
		a1.Logger.Debug("key   : %+v", instancekey)
		success, err := rh.db.SetIf(a1MediatorNs, instancekey, instanceMap[instancekey], string(data))
		if err != nil {
			a1.Logger.Error("error2 :%+v", err)
			return operation, err
		}
		if !success {
			a1.Logger.Debug("Policy instance %+v already exist", policyInstanceID)
			return operation, InstanceAlreadyError
		}

		if len(notificationDestination) > 0 {
			if err = rh.db.Set(a1MediatorNs, notificationDestinationkey, notificationDestination); err != nil {
				a1.Logger.Error("error3 :%+v", err)
				return operation, err
			}
		}
	} else {
		data, _ := json.Marshal(httpBody)
		a1.Logger.Debug("Marshaled String : %+v", string(data))
		a1.Logger.Debug("key   : %+v", instancekey)

		var instance_map []interface{}
		instance_map = append(instance_map, instancekey, string(data))
		a1.Logger.Debug("policyinstancetype map : %+v", instance_map[1])
		a1.Logger.Debug("policyinstancetype to create : %+v", instance_map)

		if err = rh.db.Set(a1MediatorNs, instancekey, string(data)); err != nil {
			a1.Logger.Error("error4 :%+v", err)
			return operation, err
		}
		if len(notificationDestination) > 0 {
			if err := rh.db.Set(a1MediatorNs, notificationDestinationkey, notificationDestination); err != nil {
				a1.Logger.Error("error :%+v", err)
				return operation, err
			}
		}
	}
	a1.Logger.Debug("Policy Instance created ")
	return operation, nil
}

func (rh *Resthook) storePolicyInstanceMetadata(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID) (bool, error) {

	creation_timestamp := time.Now()
	instanceMetadataKey := a1InstanceMetadataPrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + string(policyInstanceID)

	a1.Logger.Debug("key : %+v", instanceMetadataKey)

	var metadatajson []interface{}
	metadatajson = append(metadatajson, map[string]string{"created_at": creation_timestamp.Format("2006-01-02 15:04:05"), "has_been_deleted": "False"})
	metadata, _ := json.Marshal(metadatajson)

	a1.Logger.Debug("policyinstanceMetaData to create : %+v", string(metadata))

	err := rh.db.Set(a1MediatorNs, instanceMetadataKey, string(metadata))

	if err != nil {
		a1.Logger.Error("error :%+v", err)
		return false, err
	}

	a1.Logger.Debug("Policy Instance Meta Data created at :%+v", creation_timestamp)

	return true, nil
}

func (rh *Resthook) CreatePolicyInstance(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID, httpBody interface{}, notificationDestination string) error {
	a1.Logger.Debug("CreatePolicyInstance function")
	//  validate the PUT against the schema
	var policyTypeSchema *models.PolicyTypeSchema
	policyTypeSchema, err := rh.GetPolicyType(policyTypeId)
	if err != nil {
		a1.Logger.Error("error : %+v", err)
		return err
	}
	schemaStr, err := json.Marshal(policyTypeSchema.CreateSchema)
	if err != nil {
		a1.Logger.Error("Json Marshal error : %+v", err)
		return err
	}
	a1.Logger.Debug("schema to validate %+v", string(schemaStr))
	a1.Logger.Debug("httpbody to validate %+v", httpBody)
	schemaString := fmt.Sprint(string(schemaStr))
	httpBodyMarshal, err := json.Marshal(httpBody)
	httpBodyString := string((httpBodyMarshal))
	a1.Logger.Debug("schema to validate sprint  %+v", (schemaString))
	a1.Logger.Debug("httpbody to validate sprint %+v", httpBodyString)
	isvalid := validate(httpBodyString, schemaString)
	if isvalid {
		var operation string
		operation, err = rh.storePolicyInstance(policyTypeId, policyInstanceID, httpBody, notificationDestination)
		if err != nil {
			a1.Logger.Error("error :%+v", err)
			return err
		}
		a1.Logger.Debug("policy instance :%+v", operation)
		iscreated, errmetadata := rh.storePolicyInstanceMetadata(policyTypeId, policyInstanceID)
		if errmetadata != nil {
			a1.Logger.Error("error :%+v", errmetadata)
			return errmetadata
		}
		if iscreated {
			a1.Logger.Debug("policy instance metadata created")
		}

		message := rmr.Message{}
		rmrMessage, err := message.PolicyMessage(strconv.FormatInt((int64(policyTypeId)), 10), string(policyInstanceID), httpBodyString, operation)
		if err != nil {
			a1.Logger.Error("error : %v", err)
			return err
		}
		isSent := rh.iRmrSenderInst.RmrSendToXapp(rmrMessage, a1PolicyRequest, int(policyTypeId))
		if isSent {
			a1.Logger.Debug("rmrSendToXapp : message sent")
		} else {
			a1.Logger.Debug("rmrSendToXapp : message not sent")
		}

	} else {
		a1.Logger.Error("%+v", invalidJsonSchema)
		return invalidJsonSchema
	}

	return nil
}

func (rh *Resthook) GetPolicyInstance(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID) (map[string]interface{}, error) {
	a1.Logger.Debug("GetPolicyInstance1")

	var keys [1]string

	typekey := a1PolicyPrefix + strconv.FormatInt((int64(policyTypeId)), 10)
	keys[0] = typekey

	a1.Logger.Debug("key1 : %+v", typekey)

	valmap, err := rh.db.Get(a1MediatorNs, keys[:])
	if len(valmap) == 0 {
		a1.Logger.Debug("policy type Not Present for policyid : %v", policyTypeId)
		return map[string]interface{}{}, policyTypeNotFoundError
	}

	if err != nil {
		a1.Logger.Error("error in retrieving policy type. err: %v", err)
		return map[string]interface{}{}, err
	}

	if valmap[typekey] == nil {
		a1.Logger.Debug("policy type Not Present for policyid : %v", policyTypeId)
		return map[string]interface{}{}, policyTypeNotFoundError
	}

	a1.Logger.Debug("keysmap : %+v", valmap[typekey])

	instancekey := a1InstancePrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + string(policyInstanceID)
	a1.Logger.Debug("key2 : %+v", instancekey)
	keys[0] = instancekey
	instanceMap, err := rh.db.Get(a1MediatorNs, keys[:])
	if err != nil {
		a1.Logger.Error("policy instance error : %v", err)
	}
	a1.Logger.Debug("policyinstancetype map : %+v", instanceMap)

	if instanceMap[instancekey] == nil {
		a1.Logger.Debug("policy instance Not Present for policyinstaneid : %v", policyInstanceID)
		return map[string]interface{}{}, policyInstanceNotFoundError
	}

	var valStr map[string]interface{}
	err = json.Unmarshal([]byte(instanceMap[instancekey].(string)), &valStr)
	if err != nil {
		fmt.Println("error:", err)
	}
	fmt.Println(valStr)
	return valStr, nil
}

func (rh *Resthook) GetAllPolicyInstance(policyTypeId models.PolicyTypeID) ([]models.PolicyInstanceID, error) {
	a1.Logger.Debug("GetAllPolicyInstance")
	var policyTypeInstances = []models.PolicyInstanceID{}

	keys, err := rh.db.GetAll("A1m_ns")

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
	}

	a1.Logger.Debug("return : %+v", policyTypeInstances)
	return policyTypeInstances, nil
}

func (rh *Resthook) DeletePolicyType(policyTypeId models.PolicyTypeID) error {
	a1.Logger.Debug("DeletePolicyType")

	policyinstances, err := rh.GetAllPolicyInstance(policyTypeId)
	if err != nil {
		a1.Logger.Error("error in retrieving policy. err: %v", err)
		return err
	}

	var keys [1]string
	key := a1PolicyPrefix + strconv.FormatInt((int64(policyTypeId)), 10)
	keys[0] = key
	if len(policyinstances) == 0 {
		err := rh.db.Remove(a1MediatorNs, keys[:])
		if err != nil {
			a1.Logger.Error("error in deleting policy type err: %v", err)
			return err
		}
	} else {
		a1.Logger.Error("tried to delete a type that isn't empty")
		return policyTypeCanNotBeDeletedError
	}
	return nil
}

func (rh *Resthook) typeValidity(policyTypeId models.PolicyTypeID) error {
	var keys [1]string

	typekey := a1PolicyPrefix + strconv.FormatInt((int64(policyTypeId)), 10)
	keys[0] = typekey

	a1.Logger.Debug("key1 : %+v", typekey)
	valmap, err := rh.db.Get(a1MediatorNs, keys[:])
	if err != nil {
		a1.Logger.Error("error in retrieving policytype err: %v", err)
		return err
	}
	if len(valmap) == 0 {
		a1.Logger.Error("policy type Not Present for policyid : %v", policyTypeId)
		return policyTypeNotFoundError
	}
	return nil
}

func (rh *Resthook) instanceValidity(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID) error {
	err := rh.typeValidity(policyTypeId)
	if err != nil {
		return err
	}
	policyTypeInstances, err := rh.GetPolicyInstance(policyTypeId, policyInstanceID)
	if err != nil {
		a1.Logger.Error("policy instance error : %v", err)
		return err
	}
	if policyTypeInstances == nil {
		a1.Logger.Debug("policy instance Not Present  ")
		return policyInstanceNotFoundError
	}
	return nil
}

func (rh *Resthook) getMetaData(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID) (map[string]interface{}, error) {
	instanceMetadataKey := a1InstanceMetadataPrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + string(policyInstanceID)
	a1.Logger.Debug("instanceMetadata key : %+v", instanceMetadataKey)
	var keys [1]string
	keys[0] = instanceMetadataKey
	instanceMetadataMap, err := rh.db.Get(a1MediatorNs, keys[:])
	if err != nil {
		a1.Logger.Error("policy instance error : %v", err)
	}
	a1.Logger.Debug("instanceMetadata map : %+v", instanceMetadataMap)
	if instanceMetadataMap[instanceMetadataKey] == nil {
		a1.Logger.Error("policy instance Not Present for policyinstaneid : %v", policyInstanceID)
		return map[string]interface{}{}, policyInstanceNotFoundError
	}
	return instanceMetadataMap, nil
}

func (rh *Resthook) getPolicyInstanceStatus(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID) (bool, error) {
	instancehandlerKey := a1HandlerPrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + string(policyInstanceID)
	var keys [1]string
	keys[0] = instancehandlerKey
	resp, err := rh.db.Get(a1MediatorNs, keys[:])
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

func (rh *Resthook) GetPolicyInstanceStatus(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID) (*a1_mediator.A1ControllerGetPolicyInstanceStatusOKBody, error) {
	err := rh.instanceValidity(policyTypeId, policyInstanceID)
	policyInstanceStatus := a1_mediator.A1ControllerGetPolicyInstanceStatusOKBody{}
	policyInstanceStatus.EnforceStatus = "NOT_ENFORCED"
	policyInstanceStatus.EnforceReason = "OTHER_REASON"
	if err != nil && (err == policyInstanceNotFoundError || err == policyTypeNotFoundError) {
		return &policyInstanceStatus, err
	}
	metadata, err := rh.getMetaData(policyTypeId, policyInstanceID)
	a1.Logger.Debug(" metadata %v", metadata)
	if err != nil {
		a1.Logger.Error("policy instance error : %v", err)
		return &policyInstanceStatus, err
	}
	jsonbody, err := json.Marshal(metadata)
	if err != nil {
		a1.Logger.Error("marshal error : %v", err)
		return &policyInstanceStatus, err
	}

	if err := json.Unmarshal(jsonbody, &policyInstanceStatus); err != nil {
		a1.Logger.Error("unmarshal error : %v", err)
		//this error maps to 503 error but can be mapped to 500: internal error
		return &policyInstanceStatus, err
	}
	enforced, err := rh.getPolicyInstanceStatus(policyTypeId, policyInstanceID)
	if err != nil || (err == nil && !enforced) {
		a1.Logger.Error("marshal error : %v", err)
		return &policyInstanceStatus, err
	}
	policyInstanceStatus.EnforceStatus = "ENFORCED"
	policyInstanceStatus.EnforceReason = ""
	return &policyInstanceStatus, nil
}

func (rh *Resthook) storeDeletedPolicyInstanceMetadata(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID, creation_timestamp string) error {
	deleted_timestamp := time.Now()

	instanceMetadataKey := a1InstanceMetadataPrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + string(policyInstanceID)

	a1.Logger.Debug("instanceMetadata Key : %+v", instanceMetadataKey)

	var metadatajson interface{}
	metadatajson = map[string]string{"created_at": creation_timestamp, "has_been_deleted": "True", "deleted_at": deleted_timestamp.Format("2006-01-02 15:04:05")}
	a1.Logger.Debug("metadatajson to create : %+v", metadatajson)
	deletedmetadata, err := json.Marshal(metadatajson)

	a1.Logger.Debug("policyinstanceMetaData to create : %+v", string(deletedmetadata))

	err = rh.db.Set(a1MediatorNs, instanceMetadataKey, string(deletedmetadata))
	a1.Logger.Debug("deletemetadatacreated")
	if err != nil {
		a1.Logger.Error("error :%+v", err)
		return err
	}

	a1.Logger.Error("Policy Instance Meta Data deleted at :%+v", creation_timestamp)

	return nil
}

func (rh *Resthook) deleteInstancedata(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID) error {
	var keys [1]string
	instancekey := a1InstancePrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + string(policyInstanceID)
	keys[0] = instancekey
	err := rh.db.Remove(a1MediatorNs, keys[:])
	if err != nil {
		a1.Logger.Error("error in deleting policy instance err: %v", err)
		return err
	}
	return nil
}

func (rh *Resthook) deleteNotificationDestination(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID) error {
	var keys [1]string
	notificationDestinationkey := a1NotificationDestinationPrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + string(policyInstanceID)
	keys[0] = notificationDestinationkey
	err := rh.db.Remove(a1MediatorNs, keys[:])
	if err != nil {
		a1.Logger.Error("error in deleting notificationDestination err: %v", err)
		return err
	}

	return nil
}

func (rh *Resthook) deleteMetadata(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID) error {
	var keys [1]string
	instanceMetadataKey := a1InstanceMetadataPrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + string(policyInstanceID)

	a1.Logger.Debug("instanceMetadata Key : %+v", instanceMetadataKey)
	keys[0] = instanceMetadataKey
	err := rh.db.Remove(a1MediatorNs, keys[:])
	if err != nil {
		a1.Logger.Error("error in deleting policy metadata err: %v", err)
		return err
	}
	return nil
}

func (rh *Resthook) DeletePolicyInstance(policyTypeId models.PolicyTypeID, policyInstanceID models.PolicyInstanceID) error {
	err := rh.instanceValidity(policyTypeId, policyInstanceID)
	if err != nil {
		a1.Logger.Error("policy instance error : %v", err)
		if err == policyInstanceNotFoundError || err == policyTypeNotFoundError {
			return err
		}
	}

	createdmetadata, err := rh.getMetaData(policyTypeId, policyInstanceID)
	if err != nil {
		a1.Logger.Error("error : %v", err)
		return err
	}
	a1.Logger.Debug(" created metadata %v", createdmetadata)
	instanceMetadataKey := a1InstanceMetadataPrefix + strconv.FormatInt((int64(policyTypeId)), 10) + "." + string(policyInstanceID)
	creation_metadata := createdmetadata[instanceMetadataKey]
	var metadata map[string]interface{}
	creation_metadata_string := creation_metadata.(string)
	creation_metadata_string = strings.TrimRight(creation_metadata_string, "]")
	creation_metadata_string = strings.TrimLeft(creation_metadata_string, "[")
	if err = json.Unmarshal([]byte(creation_metadata_string), &metadata); err != nil {
		a1.Logger.Error("unmarshal error : %v", err)
		return err
	}

	a1.Logger.Debug(" created metadata created_at %v", metadata["created_at"])
	creation_timestamp := metadata["created_at"]

	rh.deleteMetadata(policyTypeId, policyInstanceID)

	rh.deleteInstancedata(policyTypeId, policyInstanceID)

	rh.deleteNotificationDestination(policyTypeId, policyInstanceID)

	rh.storeDeletedPolicyInstanceMetadata(policyTypeId, policyInstanceID, creation_timestamp.(string))

	message := rmr.Message{}
	rmrMessage, err1 := message.PolicyMessage(strconv.FormatInt((int64(policyTypeId)), 10), string(policyInstanceID), "", "DELETE")
	if err1 != nil {
		a1.Logger.Error("error : %v", err1)
		return err1
	}
	isSent := rh.iRmrSenderInst.RmrSendToXapp(rmrMessage, a1PolicyRequest, int(policyTypeId))
	if isSent {
		a1.Logger.Debug("rmrSendToXapp : message sent")
	} else {
		//TODO:if message not sent need to return error or just log it or retry sending
		a1.Logger.Error("rmrSendToXapp : message not sent")
	}

	return nil
}

func (rh *Resthook) DataDelivery(httpBody interface{}) error {
	a1.Logger.Debug("httpbody : %+v", httpBody)
	mymap := httpBody.(map[string]interface{})
	message := rmr.Message{}
	rmrMessage, err := message.A1EIMessage(mymap["job"].(string), mymap["payload"].(string))
	if err != nil {
		a1.Logger.Error("error : %v", err)
		return err
	}
	a1.Logger.Debug("rmrSendToXapp :rmrMessage %+v", rmrMessage)
	isSent := rh.iRmrSenderInst.RmrSendToXapp(rmrMessage, a1EIDataDelivery, rmr.DefaultSubId)
	if isSent {
		a1.Logger.Debug("rmrSendToXapp : message sent")
	} else {
		a1.Logger.Error("rmrSendToXapp : message not sent")
	}
	return nil
}
