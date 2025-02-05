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
        <h2>{Board 1}</h2>
        <ol>
            <li>Temperature: {Board 1 temperature} F</li>
            <li>Humidity: {Board 1 humidity} %</li>
            <li>Pressure: {Board 1 pressure} hPa</li>
            <li>PM 1.0: {Board 1 pm1_0} ppm</li>
            <li>PM 2.5: {Board 1 pm2_5} ppm</li>
            <li>PM 10.0: {Board 1 pm10} ppm</li>
        </ol>
    </div>
    <div class="board">
        <h2>{Board 2}</h2>
        <ol>
            <li>Temperature: {Board 2 temperature} F</li>
            <li>Humidity: {Board 2 humidity} 5</li>
            <li>Pressure: {Board 2 pressure} hPa</li>
            <li>PM 1.0: {Board 2 pm1_0} ppm</li>
            <li>PM 2.5: {Board 2 pm2_5} ppm</li>
            <li>PM 10.0: {Board 2 pm10} ppm</li>
        </ol>
    </div>
    <div class="board">
        <h2>{Board 3}</h2>
        <ol>
            <li>Temperature: {Board 3 temperature} F</li>
            <li>Humidity: {Board 3 humidity} %</li>
            <li>Pressure: {Board 3 pressure} hPa</li>
            <li>PM 1.0: {Board 3 pm1_0} ppm</li>
            <li>PM 2.5: {Board 3 pm2_5} ppm</li>
            <li>PM 10.0: {Board 3 pm10} ppm</li>
        </ol>
    </div>
    <div class="board">
        <h2>{Board 4}</h2>
        <ol>
            <li>Temperature: {Board 4 temperature} F</li>
            <li>Humidity: {Board 4 humidity} 5</li>
            <li>Pressure: {Board 4 pressure} hPa</li>
            <li>PM 1.0: {Board 4 pm1_0} ppm</li>
            <li>PM 2.5: {Board 4 pm2_5} ppm</li>
            <li>PM 10.0: {Board 4 pm10} ppm</li>
        </ol>
    </div>
</div>
<p>Made By Chris Londal</p>
</body>
</html>

)rawliteral";