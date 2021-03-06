﻿/*
    ロータリーエンコーダーとボタンをHIDマウスにする

    コンパイル環境　： Arduino IDE(Arduino 1.0.5)
    ターゲットボード： [Tools]→[Board]→[Arduino Leonardo]
    
    開発ボード　　　：SparkFun Pro Micro - 5V/16MHz

                                                @ichirowo
    2014/09/23    
    
*/

//入力ピン設定//////////////////////////////////////////
#define LclickPin 16      //左クリック入力ピン
#define RclickPin 15      //右クリック入力ピン
#define HorizonApin 2     //エンコーダー水平A入力ピン
#define HorizonBpin 3     //エンコーダー水平B入力ピン
#define VerticalApin 4    //エンコーダー垂直A入力ピン
#define VerticalBpin 5    //エンコーダー垂直B入力ピン

//グローバル変数////////////////////////////////////////

//左クリック
boolean LclickNow = false;    //左クリック信号OUT
int LclickJudg = 0;           //左クリックカウンタ
int LclickTime = 0;           //左クリックタイマ

//右クリック
boolean RclickNow = false;    //右クリック信号OUT
int RclickJudg = 0;           //右クリックカウンタ
int RclickTime = 0;           //右クリックタイマ

//水平
char H_state;	     //エンコーダー2bit→4進数（0～3）
char H_dir;          //回転方向（0:変化無し 1:左回り 2:右回り 9:エラー）
int H_RollMode = 0;  //回転方向固定（1:左回り 2:右回り）
int H_RollTime = 0;  //無回転中のカウンタ

//垂直
char V_state;
char V_dir;
int V_RollMode = 0;
int V_RollTime = 0;

//マウスホイール
char W_state;
char W_dir;
int W_RollMode = 0;
int W_RollTime = 0;
boolean WclickNow = false;   //マウスホイール機能発動


void setup() {
    //入力ピン設定
    pinMode(LclickPin,INPUT_PULLUP);
    pinMode(RclickPin,INPUT_PULLUP);
    pinMode(HorizonApin,INPUT);
    pinMode(HorizonBpin,INPUT);
    pinMode(VerticalApin,INPUT);
    pinMode(VerticalBpin,INPUT);

    //マウスにする
    Mouse.begin();
}

void loop() {
    //クリックシーケンス====================================================

    if(digitalRead(LclickPin) == LOW && digitalRead(RclickPin) == LOW){
        //両クリック（マウスホイールON）
        Mouse.release(MOUSE_LEFT);
        Mouse.release(MOUSE_RIGHT);
        WclickNow = true;
    }else{
        WclickNow = false;
        //左クリック
        if(digitalRead(LclickPin) == LOW){
            LclickJudg++;
            LclickNow = true;
        }
        if(LclickNow == true){
            LclickTime++;
            if(LclickTime > 100){
                if(LclickJudg > 50){
                    LclickTime = 0;
                    LclickJudg = 0;
                    Mouse.press(MOUSE_LEFT);
                }else{
                    LclickTime = 0;
                    LclickJudg = 0;
                    Mouse.release(MOUSE_LEFT);
                    LclickNow = false;
                }
            }
        }
        
        //右クリック
        if(digitalRead(RclickPin) == LOW){
            RclickJudg++;
            RclickNow = true;
        }
        if(RclickNow == true){
            RclickTime++;
            if(RclickTime > 100){
                if(RclickJudg > 50){
                    RclickTime = 0;
                    RclickJudg = 0;
                    Mouse.press(MOUSE_RIGHT);
                }else{
                    RclickTime = 0;
                    RclickJudg = 0;
                    Mouse.release(MOUSE_RIGHT);
                    RclickNow = false;
                }
            }
        }
    }
    
    //ロータリーエンコーダーシーケンス======================================
    if(WclickNow == true){
        // マウスホイールの機能-----------------------

        //回転判別（垂直方向のマウスホイール）
        char W_state_before = W_state;
        W_state = rotary_getState(VerticalApin, VerticalBpin); // 今の状態
        W_dir = rotary_getDir(W_state, W_state_before); // 回転方向
        
        //止まっていたら、移動方向をリセット（垂直方向のマウスホイール）
        if(W_dir == 0){
            W_RollTime++;
            if(W_RollTime > 50){
                W_RollMode = 0;
            }
        }else{
            W_RollTime = 0;
        }

        //マウスの移動方向をセット（垂直方向のマウスホイール）
        if(W_RollMode == 0){
            if(W_dir == 1){
                W_RollMode = 1;
            }else if(W_dir == 2){
                W_RollMode = 2;
            }
        }

        //マウスを動かす（垂直方向のマウスホイール）
        if(W_RollMode == 1){
            switch(W_dir){
                case 1:
                    Mouse.move(0, 0, 1);
                break;
                case 9:
                    Mouse.move(0, 0, 1);
                break;
                case 2:
                    Mouse.move(0, 0, 1);
                break;
            }
        }else if(W_RollMode == 2){
            switch(W_dir){
                case 2:
                    Mouse.move(0, 0, -1);
                break;
                case 9:
                    Mouse.move(0, 0, -1);
                break;
                case 1:
                    Mouse.move(0, 0, -1);
                break;
            }
        }
    }else{
        // 普通のマウス機能-----------------------

	//回転判別（水平方向）
        char H_state_before = H_state;
        H_state = rotary_getState(HorizonApin, HorizonBpin); // 今の状態
        H_dir = rotary_getDir(H_state, H_state_before); // 回転方向
        
        //止まっていたら、移動方向をリセット（水平方向）
        if(H_dir == 0){
            H_RollTime++;
            if(H_RollTime > 50){
                H_RollMode = 0;
            }
        }else{
            H_RollTime = 0;
        }
        
        //マウスの移動方向をセット（水平方向）
        if(H_RollMode == 0){
            if(H_dir == 1){
                H_RollMode = 1;
            }else if(H_dir == 2){
                H_RollMode = 2;
            }
        }

        //マウスを動かす（水平方向）
        if(H_RollMode == 1){
            switch(H_dir){
                case 1:
                    Mouse.move(1, 0, 0);
                break;
                case 9:
                    Mouse.move(2, 0, 0);
                break;
                case 2:
                    Mouse.move(25, 0, 0);
                break;
            }
        }else if(H_RollMode == 2){
            switch(H_dir){
                case 2:
                    Mouse.move(-1, 0, 0);
                break;
                case 9:
                    Mouse.move(-2, 0, 0);
                break;
                case 1:
                    Mouse.move(-25, 0, 0);
                break;
            }
        }
    
        //回転判別（垂直方向）
        char V_state_before = V_state;
        V_state = rotary_getState(VerticalApin, VerticalBpin); // 今の状態
        V_dir = rotary_getDir(V_state, V_state_before); // 回転方向
        
        //止まっていたら、移動方向をリセット（垂直方向）
        if(V_dir == 0){
            V_RollTime++;
            if(V_RollTime > 50){
                V_RollMode = 0;
            }
        }else{
            V_RollTime = 0;
        }

        //マウスの移動方向をセット（垂直方向）
        if(V_RollMode == 0){
            if(V_dir == 1){
                V_RollMode = 1;
            }else if(V_dir == 2){
                V_RollMode = 2;
            }
        }

        //マウスを動かす（垂直方向）
        if(V_RollMode == 1){
            switch(V_dir){
                case 1:
                    Mouse.move(0, -1, 0);
                break;
                case 9:
                    Mouse.move(0, -2, 0);
                break;
                case 2:
                    Mouse.move(0, -20, 0);
                break;
            }
        }else if(V_RollMode == 2){
            switch(V_dir){
                case 2:
                    Mouse.move(0, 1, 0);
                break;
                case 9:
                    Mouse.move(0, 2, 0);
                break;
                case 1:
                    Mouse.move(0, 20, 0);
                break;
            }
        }
    }
}

//ロータリーエンコーダの2ビット→10進数変換
char rotary_getState(char pinA, char pinB){
    if(digitalRead(pinA)){
        if(digitalRead(pinB)) return 2;
        else return 0;
    }else{
        if(digitalRead(pinB)) return 1;
        else return 3;
    }
}
//1つ前の状態と比較して回転方向を取得
char rotary_getDir(int state, int past_state){
    if((state+3+1)%3 == past_state)      return 1;    // 左回り
    else if((state+3-1)%3 == past_state) return 2;    // 右回り
    else if(state == past_state)         return 0;    // 変化無し
    return 9;                                         //　エラー
}
