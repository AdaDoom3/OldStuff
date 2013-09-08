using UnityEngine;
using System.Collections;
//[AddComponentMenu("Camera-Control/Mouse Look")]
public class PlayerMovementv3 : MonoBehaviour{
	//
	// Types
	//
		//public
		public enum enumInput{
			keyboard,
			stickXbox,
			stickPs3,
			screenIos,
			screenAndroid
		}
	//
	// Variables
	//
		//public
		public int accelerationStart = 8; //Higher the slower
		public float distanceFromCamera = 0.6f;
		public float maximumSpeed = 0.5f;
		public enumInput input = enumInput.cursorMouse;
		public Vector2 sensitivityKeyboard = new Vector2(0.003f, 0.002f);
		public Vector2 sensitivityScreen = new Vector2(0.16f, 0.16f);
		public Vector2 sensitivityStick = new Vector2(1.0f, 1.0f);
		public Vector3 angleMaximum = new Vector3(20.0f, 20.0f, 45.0f);
		public Vector2 angleAimModifer = new Vector2(20.0f, 25.0f);
		public Vector2 movementBoundries = new Vector2(0.8f, 0.5f);
		//private
		private bool canMove = true;
		private Vector2 positionTargetNext = new Vector2(0.0f, 0.0f);
		private Vector2 positionTargetPrevious = new Vector2(0.0f, 0.0f);
		private Vector2 positionTarget = new Vector2(0.0f, 0.0f);
		private Vector2 position = new Vector2(0.0f, 0.0f);
		private Transform cameraTransform;
	//
	// Functions
	//
		void Awake(){
			Screen.lockCursor = true;
			cameraTransform = transform.parent.Find("PlayerCamera");
		}
		void Start(){
			acceleration = accelerationStart;
		}
		void Update(){
			if(Input.GetKeyDown("escape")){
				if(canMove){
					Screen.lockCursor = false;
					canMove = false;
				}else{
					Screen.lockCursor = true;
					canMove = true;
				}
			}
			if(canMove){
				switch(input){
					case enumInput.keyboard:
						positionTargetNext.x += Input.GetAxis("Mouse X") * sensitivityMouse.x;
						positionTargetNext.y += Input.GetAxis("Mouse Y") * sensitivityMouse.y;
						break;/*
					case enumInput.iosScreen:
						break;
					case enumInput.androidScreen:
						break;*/
					case enumInput.stickXbox:
						positionTargetNext.x += Input.GetAxis("Horizontal") * sensitivityStick.x;
						positionTargetNext.y -= Input.GetAxis("Vertical") * sensitivityStick.y;
						break;
				}
				for(int i = 0;i < 2;i++){
					if(positionTargetNext != positionTargetPrevious){
						acceleration = accelerationStart;
					}else{
						if(acceleration > 1){
							acceleration--;
						}
						if(positionTargetNext[i] > movementBoundries[i]){
							positionTargetNext[i] -= maximumSpeed * 0.04f;
						}else if(positionTargetNext[i] < -movementBoundries[i]){
							positionTargetNext[i] += maximumSpeed * 0.04f;
						}
					}
				}
				transform.localPosition = new Vector3(
					position.x,
					position.y,
					distanceFromCamera
				);/*
				transform.localEulerAngles = new Vector3(
					-((positionTarget.y - position.y) * angleAmplifier.y + position.y * angleAimModifer.y),
					 ((positionTarget.x - position.x) * angleAmplifier.x + position.x * angleAimModifer.x),
					-((positionTarget.x - position.x) * angleAmplifier.z)
				);*/
				cameraTransform.localEulerAngles = new Vector3(
					0,
					0,
					-((positionTarget.x - position.x) * angleAmplifier.z * 0.3f)
				);/*
				cameraTransform.localPosition = new Vector3(
					position.x * 0.85f,
					position.y * 0.83f,
					0
				);*/
				positionTargetPrevious = positionTargetNext;
			}
		}
	}
