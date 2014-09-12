/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.2

Page {
    Container {
        layout: DockLayout {

        }

        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Center
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom

            }
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            Button {
                text: "Update profile"
                onClicked: {
                    app.sendProfileUpdate()
                }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center

            }
            
            Button {
                text: "Update super properties"
                onClicked: {
                    app.updateSuperProperties();
                }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }

            Button {
                text: "Track Level complete"
                onClicked: {
                    var eventPorperties = {
                        "Level Number" : 9,
                        "Coins" : 1250
                    }
                    mixpanel.trackEvent("Level Complete", eventPorperties);                    
                }
            }
            
            

            Button {
                text: "Increment Coins by 10"
                onClicked: {
                    app.sendCoinsIncrement();
                }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }



            Button {
                text: "Flush mixpanel messages"
                onClicked: {
                    app.flushMixpanelMessages();
                }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }
            
            Button {
                text: "Force engage profile error"
                onClicked: {
                    app.forceEngageProfileError()
                }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            
            }
            
            Button {
                text: "Force track event error"
                onClicked: {
                    app.forceTrackEventError()
                }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            
            }
            
            Button {
                text: "Delete profile"
                onClicked: {
                    mixpanel.deleteProfile();
                }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            
            }

        }
    }
}
