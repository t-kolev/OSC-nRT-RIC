//
// Copyright 2019 AT&T Intellectual Property
// Copyright 2019 Nokia
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//  This source code is part of the near-RT RIC (RAN Intelligent Controller)
//  platform project (RICP).


package common

import (
	"fmt"
	"github.com/stretchr/testify/assert"
	"testing"
)

func TestNewResourceNotFoundError(t *testing.T){
	msg := "Expected error"
	expectedErr := NewResourceNotFoundError(msg)
	assert.NotNil(t, expectedErr)
	assert.IsType(t, &ResourceNotFoundError{}, expectedErr)
	assert.Contains(t, expectedErr.Error(), msg)
}

func TestNewResourceNotFoundErrorf(t *testing.T){
	msg := "Expected error: %s, %s"
	var args []interface{}
	args = append(args, "arg1", "arg2")
	expectedErr := NewResourceNotFoundErrorf(msg, args...)
	assert.NotNil(t, expectedErr)
	assert.IsType(t, &ResourceNotFoundError{}, expectedErr)
	assert.Contains(t, expectedErr.Error(), fmt.Sprintf(msg, args...))
}