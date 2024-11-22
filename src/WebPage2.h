const char* webpage2 = R"rawliteral(

<!DOCTYPE html>
<html>
<head>
<title>ESP32 Web Server</title>
<script>
function autoRefresh() {
    window.location = window.location.href;
}
setInterval('autoRefresh()', 20000);
</script>
<style>
    body {
        font-family: 'Times New Roman', serif;
    }
    .boards-container {
        display: flex;
        justify-content: space-between;
        margin: 20px;
    }
    .board {
        border: 1px solid #ccc;
        padding: 10px;
        border-radius: 5px;
        width: 23%; /* Adjust width as necessary */
        box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1);
    }
    .board h2 {
        color: #2e6c80;
        text-align: center;
    }
    ol {
        list-style: none;
        padding: 0;
        margin: 0;
        font-size: 14px;
        line-height: 1.6;
        font-weight: bold;
    }
    .nav-buttons a {
        background-color: #2b2301;
        color: #fff;
        display: inline-block;
        padding: 3px 10px;
        font-weight: bold;
        border-radius: 5px;
        margin-right: 5px;
        text-decoration: none;
    }
    .nav-buttons a:hover {
        background-color: #444;
    }
</style>
</head>
<body>
<h1 style="color: #5e9ca0;">ESP32 Web Server</h1>
<div class="nav-buttons">
    <a href="/">Home Page</a>
    <a href="/test">Test Page</a>
    <a href="/test2">Test Page 2</a>
    <a href="/stop">Stop Server</a>
</div>
<h2 style="color: #2e6c80;">Boards:</h2>
<div class="boards-container">
    <div class="board">
        <h2>1129945228</h2>
        <ol>
            <li>Temperature: {1129945228 temperature}</li>
            <li>Humidity: {1129945228 humidity}</li>
            <li>Pressure: {1129945228 pressure}</li>
            <li>PM 1.0: {1129945228 pm1_0}</li>
            <li>PM 2.5: {1129945228 pm2_5}</li>
            <li>PM 10.0: {1129945228 pm10}</li>
        </ol>
    </div>
    <div class="board">
        <h2>1129948912</h2>
        <ol>
            <li>Temperature: {1129948912 temperature}</li>
            <li>Humidity: {1129948912 humidity}</li>
            <li>Pressure: {1129948912 pressure}</li>
            <li>PM 1.0: {1129948912 pm1_0}</li>
            <li>PM 2.5: {1129948912 pm2_5}</li>
            <li>PM 10.0: {1129948912 pm10}</li>
        </ol>
    </div>
    <div class="board">
        <h2>Board 3</h2>
        <ol>
            <li>Temperature: {Board 3 temperature}</li>
            <li>Humidity: {Board 3 humidity}</li>
            <li>Pressure: {Board 3 pressure}</li>
            <li>PM 1.0: {Board 3 pm1_0}</li>
            <li>PM 2.5: {Board 3 pm2_5}</li>
            <li>PM 10.0: {Board 3 pm10}</li>
        </ol>
    </div>
    <div class="board">
        <h2>Board 4</h2>
        <ol>
            <li>Temperature: {Board 4 temperature}</li>
            <li>Humidity: {Board 4 humidity}</li>
            <li>Pressure: {Board 4 pressure}</li>
            <li>PM 1.0: {Board 4 pm1_0}</li>
            <li>PM 2.5: {Board 4 pm2_5}</li>
            <li>PM 10.0: {Board 4 pm10}</li>
        </ol>
    </div>
</div>
<p>Made By Chris Londal</p>
</body>
</html>

)rawliteral";