const char* webpage = R"rawliteral(

<!DOCTYPE html>
<html><head>
<title>ESP32 Web Server</title>
<script>function autoRefresh() {window.location = window.location.href;} setInterval('autoRefresh()', 20000);</script>
<h1 style="color: #5e9ca0;">ESP32 Web Server</h1>
<p><a href="/"><span style="background-color: #2b2301; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;">Home Page</span></a>
<a href="/test"><span style="background-color: #2b2301; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;">Test Page</span></a>
<a href="/test2"><span style="background-color: #2b2301; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;">Test Page 2</span></a>
<a href="/stop"><span style="background-color: #2b2301; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;">Stop Server</span></a></p>
<h2 style="color: #2e6c80;">Boards:</h2>
<p>{Paragraph space}</p>
<h2 style="color: #2e6c80;">{Board 1}:</h2>
<ol style="list-style: none; font-size: 14px; line-height: 32px; font-weight: bold;">
<li style="clear: both;">Temperature: {Board 1 temperature}</li>
<li style="clear: both;">Humidity = {Board 1 humidity}</li>
<li style="clear: both;">Pressure = {Board 1 pressure}</li>
<li style="clear: both;">PM 1.0 = {Board 1 pm1_0}</li>
<li style="clear: both;">PM 2.5 = {Board 1 pm2_5}</li>
<li style="clear: both;">PM 10.0 = {Board 1 pm10}</li>
</ol>
<p>&nbsp;</p>
<h2 style="color: #2e6c80;">{Board 2}:</h2>
<ol style="list-style: none; font-size: 14px; line-height: 32px; font-weight: bold;">
<li style="clear: both;">Temperature: {Board 2 temperature}</li>
<li style="clear: both;">Humidity = {Board 2 humidity}</li>
<li style="clear: both;">Pressure = {Board 2 pressure}</li>
<li style="clear: both;">PM 1.0 = {Board 2 pm1_0}</li>
<li style="clear: both;">PM 2.5 = {Board 2 pm2_5}</li>
<li style="clear: both;">PM 10.0 = {Board 2 pm10}</li>
</ol>
<p>&nbsp;</p>
<h2 style="color: #2e6c80;">Chart View:</h2>
<table style="height: 144px; width: 100%; border-collapse: collapse;" border="1">
<tbody>
<tr style="height: 18px;">
<td style="width: 14.2857%; height: 18px;">1</td>
<td style="width: 14.2857%; height: 18px;">2</td>
<td style="width: 14.2857%; height: 18px;">3</td>
<td style="width: 14.2857%; height: 18px;">4</td>
<td style="width: 14.2857%; height: 18px;">5</td>
<td style="width: 14.2857%; height: 18px;">6</td>
<td style="width: 14.2857%; height: 18px;">7</td>
</tr>
<tr style="height: 18px;">
<td style="width: 14.2857%; height: 18px;">8</td>
<td style="width: 14.2857%; height: 18px;">9</td>
<td style="width: 14.2857%; height: 18px;">10</td>
<td style="width: 14.2857%; height: 18px;">11</td>
<td style="width: 14.2857%; height: 18px;">12</td>
<td style="width: 14.2857%; height: 18px;">13</td>
<td style="width: 14.2857%; height: 18px;">14</td>
</tr>
<tr style="height: 18px;">
<td style="width: 14.2857%; height: 18px;">15</td>
<td style="width: 14.2857%; height: 18px;">16</td>
<td style="width: 14.2857%; height: 18px;">17</td>
<td style="width: 14.2857%; height: 18px;">18</td>
<td style="width: 14.2857%; height: 18px;">19</td>
<td style="width: 14.2857%; height: 18px;">20</td>
<td style="width: 14.2857%; height: 18px;">21</td>
</tr>
<tr style="height: 18px;">
<td style="width: 14.2857%; height: 18px;">22</td>
<td style="width: 14.2857%; height: 18px;">23</td>
<td style="width: 14.2857%; height: 18px;">24</td>
<td style="width: 14.2857%; height: 18px;">25</td>
<td style="width: 14.2857%; height: 18px;">26</td>
<td style="width: 14.2857%; height: 18px;">27</td>
<td style="width: 14.2857%; height: 18px;">28</td>
</tr>
<tr style="height: 18px;">
<td style="width: 14.2857%; height: 18px;">29</td>
<td style="width: 14.2857%; height: 18px;">30</td>
<td style="width: 14.2857%; height: 18px;">31</td>
<td style="width: 14.2857%; height: 18px;">32</td>
<td style="width: 14.2857%; height: 18px;">33</td>
<td style="width: 14.2857%; height: 18px;">34</td>
<td style="width: 14.2857%; height: 18px;">35</td>
</tr>
<tr style="height: 18px;">
<td style="width: 14.2857%; height: 18px;">36</td>
<td style="width: 14.2857%; height: 18px;">37</td>
<td style="width: 14.2857%; height: 18px;">38</td>
<td style="width: 14.2857%; height: 18px;">39</td>
<td style="width: 14.2857%; height: 18px;">40</td>
<td style="width: 14.2857%; height: 18px;">41</td>
<td style="width: 14.2857%; height: 18px;">42</td>
</tr>
<tr style="height: 18px;">
<td style="width: 14.2857%; height: 18px;">43</td>
<td style="width: 14.2857%; height: 18px;">44</td>
<td style="width: 14.2857%; height: 18px;">45</td>
<td style="width: 14.2857%; height: 18px;">46</td>
<td style="width: 14.2857%; height: 18px;">47</td>
<td style="width: 14.2857%; height: 18px;">48</td>
<td style="width: 14.2857%; height: 18px;">49</td>
</tr>
<tr style="height: 18px;">
<td style="width: 14.2857%; height: 18px;">50</td>
<td style="width: 14.2857%; height: 18px;">51</td>
<td style="width: 14.2857%; height: 18px;">52</td>
<td style="width: 14.2857%; height: 18px;">53</td>
<td style="width: 14.2857%; height: 18px;">54</td>
<td style="width: 14.2857%; height: 18px;">55</td>
<td style="width: 14.2857%; height: 18px;">56</td>
</tr>
</tbody>
</table>
<p>Made By Chris Londal</p>

</body></html>

)rawliteral";