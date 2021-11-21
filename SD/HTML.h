/*HTML Defind*/
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Smart Home + ESP8266 kết nối WiFi bằng điện thoại</title>
    <style>
        body {
            font-family: "Open Sans", sans-serif;
            height: 100vh;
        }
        * {
            box-sizing: border-box;
        }
        @keyframes spinner {
            0% { transform: rotateZ(0deg); }
            100% { transform: rotateZ(359deg); }
        }
        div.wrapper {
            display: flex;
            align-items: center;
            flex-direction: column; 
            justify-content: center;
            width: 100%;
            min-height: 100%;
            padding: 20px;
            background: rgba(darken("#2196F3",40%), 0.85);
        }
        .login {
            border-radius: 2px 2px 5px 5px;
            padding: 10px 20px 20px 20px;
            width: 90%;
            max-width: 320px;
            background: #ffffff;
            position: relative;
            padding-bottom: 80px;
            box-shadow: 0px 1px 5px rgba(0,0,0,0.3);
        }
        input {
            display: block;
            padding: 15px 10px;
            margin-bottom: 10px;
            width: 100%;
            border: 1px solid #ddd;
            transition: border-width 0.2s ease;
            border-radius: 2px;
            color: #ccc;  
        }
        a {
            font-size: 0.8em;   
            color: #2196F3;
            text-decoration: none;
        }
        .title {
            color: #444;
            font-size: 1.2em;
            font-weight: bold;
            margin: 10px 0 30px 0;
            border-bottom: 1px solid #eee;
            padding-bottom: 20px;
        }
        input.state {
            width: 100%;
            height: 100%;
            padding: 10px 10px;
            background: #2196F3;
            color: #fff;
            display: block;
            border: none;
            margin-top: 20px;
            position: absolute;
            left: 0;
            bottom: 0;
            max-height: 60px;
            border: 0px solid rgba(0,0,0,0.1);
            border-radius: 0 0 2px 2px;
            transform: rotateZ(0deg);
            transition: all 0.1s ease-out;
            border-bottom-width: 7px;
        }
        .spinner {
            display: block;
            width: 40px;
            height: 40px;
            position: absolute;
            border: 4px solid #ffffff;
            border-top-color: rgba(255,255,255,0.3);
            border-radius: 100%;
            left: 50%;
            top: 0;
            opacity: 0;
            margin-left: -20px;
            margin-top: -20px;
            animation: spinner 0.6s infinite linear;
            transition: top 0.3s 0.3s ease,
                        opacity 0.3s 0.3s ease,
                        border-radius 0.3s ease;
            box-shadow: 0px 1px 0px rgba(0,0,0,0.2);
        }
        footer {
            display: block;
            padding-top: 50px;
            text-align: center;
            color: #ddd;
            font-weight: normal;
            text-shadow: 0px -1px 0px rgba(0,0,0,0.2);
            font-size: 0.8em;
        }
            
    </style>
</head>
<body>
    <div class="wrapper">
        <form class="login" action="setup">
          <p class="title">Đăng nhập wifi cho module này</p>
          <input type="text" placeholder="Tên wifi nhà bạn" autofocus name="ssid"/>
          <input type="password" placeholder="Mật khẩu wifi nhà bạn" name="password" />
          <input class="state" type="submit" value="Xác nhận"/>
        </form>
        <footer><a target="blank" href="http://google.com/">Smart Home IOT</a></footer>
        </p>
      </div>
</body>
</html>
)=====";