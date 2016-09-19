---
date: 2016-03-09T00:11:02+01:00
title: Extrinsic Calibration
weight: 30
---

## Extrinsic Calibration

<p>All sensors are extrinsically calibrated with respect to the rig body (B) reference frame (which coincides with the center GoPro C2 camera). For each sensor we give the transform <B>T</B> from the rig coordinate system to the sensor coordinate system. A left superscript indicates the coordinate system in which vectors are expressed, i.e. <SUP>C1</SUP><B>X</B> are the coordinates of vector <B>X</B> in the reference frame of camera C1. The transform <SUP>S2</SUP> <B>T</B><SUB>S1</SUB> takes a vector expressed in coordinate system S1 and transforms it to system S2: <SUP>S2</SUP><B>X</B> = <SUP>S2</SUP><B>T</B><SUB>S1</SUB> <SUP>S1</SUP><B>X</B>. For example, given coordinates in the rig reference frame (B), the ones in camera C1 can be obtained via:
<p><SUP>C1</SUP><B>X</B> = <SUP>C1</SUP><B>T</B><SUB>B</SUB> <SUP>B</SUP><B>X</B></p>
<p>The transform T can be expressed as a 3x4 transformation matrix:</p>
<p><B>T</B> = [<B>R</B>, <B>t</B>]</p>
<p>where <B>t</B> is a 3x1 translation vector, and <B>R</B> is a pure rotation.


<table>
<tr>
<td>Sensor</td><td>Transform</td><td colspan="4" class="tdcenter"><B>T</B></td></tr>
<tr><td rowspan="3">GoPro C1</td><td rowspan="3"><SUP>C1</SUP><B>T</B><SUB>B</SUB></td><td>    0.2961304418</td><td>    0.0011052645</td><td>    0.9551468682</td><td>  0.082674</td></tr>
<td>    0.0191451276</td><td>    0.9997915577</td><td>   -0.0070926152</td><td>  0.002503</td></tr>
<td>   -0.9549556144</td><td>    0.0203867479</td><td>    0.2960475553</td><td> -0.127252</td></tr>
<tr><td rowspan="3">GoPro C2</td><td rowspan="3"><SUP>C2</SUP><B>T</B><SUB>B</SUB></td><td>    1.0000000000</td><td>    0.0000000000</td><td>    0.0000000000</td><td>  0.000000</td></tr>
<td>    0.0000000000</td><td>    1.0000000000</td><td>    0.0000000000</td><td>  0.000000</td></tr>
<td>    0.0000000000</td><td>    0.0000000000</td><td>    1.0000000000</td><td>  0.000000</td></tr>
<tr><td rowspan="3">GoPro C3</td><td rowspan="3"><SUP>C3</SUP><B>T</B><SUB>B</SUB></td><td>    0.2251883426</td><td>   -0.0049445900</td><td>   -0.9743027052</td><td> -0.039557</td></tr>
<td>    0.0047482257</td><td>    0.9999808169</td><td>   -0.0039774600</td><td> -0.000781</td></tr>
<td>    0.9743036820</td><td>   -0.0037305315</td><td>    0.2252075009</td><td> -0.157049</td></tr>
<tr><td rowspan="3">Tango Bottom RGB</td><td rowspan="3"><SUP>TBR</SUP><B>T</B><SUB>B</SUB></td><td>    0.9986085337</td><td>    0.0027720771</td><td>   -0.0526622452</td><td> -0.068398</td></tr>
<td>   -0.0079486091</td><td>    0.9951207697</td><td>   -0.0983436495</td><td>  0.084056</td></tr>
<td>    0.0521326778</td><td>    0.0986253992</td><td>    0.9937581268</td><td>  0.044039</td></tr>
<tr><td rowspan="3">Tango Top Fisheye</td><td rowspan="3"><SUP>TTF</SUP><B>T</B><SUB>B</SUB></td><td>    0.9995000221</td><td>    0.0130284988</td><td>   -0.0288090975</td><td> -0.014741</td></tr>
<td>   -0.0128514581</td><td>    0.9998974312</td><td>    0.0063219540</td><td>  0.208203</td></tr>
<td>    0.0288885081</td><td>   -0.0059485542</td><td>    0.9995649398</td><td> -0.001742</td></tr>
<tr><td rowspan="3">VI Sensor Left</td><td rowspan="3"><SUP>VL</SUP><B>T</B><SUB>B</SUB></td><td>    0.9991687124</td><td>   -0.0059124048</td><td>   -0.0403351908</td><td>  0.042704</td></tr>
<td>    0.0050322682</td><td>    0.9997477793</td><td>   -0.0218873038</td><td> -0.116821</td></tr>
<td>    0.0404544240</td><td>    0.0216661317</td><td>    0.9989464542</td><td> -0.019905</td></tr>
<tr><td rowspan="3">VI Sensor Right</td><td rowspan="3"><SUP>VR</SUP><B>T</B><SUB>B</SUB></td><td>    0.9990911633</td><td>   -0.0072415591</td><td>   -0.0420048470</td><td> -0.067457</td></tr>
<td>    0.0063429185</td><td>    0.9997489915</td><td>   -0.0214877011</td><td> -0.116480</td></tr>
<td>    0.0421499079</td><td>    0.0212017389</td><td>    0.9988863156</td><td> -0.020079</td></tr>
<tr><td rowspan="3">Tango Bottom IMU</td><td rowspan="3"><SUP>TBI</SUP><B>T</B><SUB>B</SUB></td><td>    0.9986085337</td><td>    0.0027720770</td><td>   -0.0526622452</td><td> -0.006998</td></tr>
<td>    0.0192066304</td><td>   -0.9491479460</td><td>    0.3142439848</td><td> -0.074008</td></tr>
<td>   -0.0491131534</td><td>   -0.3148181892</td><td>   -0.9478804808</td><td> -0.058834</td></tr>
<tr><td rowspan="3">Tango Top IMU</td><td rowspan="3"><SUP>TTI</SUP><B>T</B><SUB>B</SUB></td><td>    0.9995000221</td><td>    0.0130284988</td><td>   -0.0288090975</td><td> -0.003344</td></tr>
<td>    0.0200390427</td><td>   -0.9658149773</td><td>    0.2584567012</td><td> -0.203341</td></tr>
<td>   -0.0244569550</td><td>   -0.2589047853</td><td>   -0.9655931698</td><td> -0.049792</td></tr>
<tr><td rowspan="3">VI Sensor IMU</td><td rowspan="3"><SUP>VI</SUP><B>T</B><SUB>B</SUB></td><td>    0.9989551739</td><td>   -0.0016800380</td><td>   -0.0456698809</td><td> -0.031005</td></tr>
<td>    0.0005817624</td><td>    0.9997105684</td><td>   -0.0240508012</td><td> -0.125714</td></tr>
<td>    0.0456970689</td><td>    0.0239991032</td><td>    0.9986670221</td><td> -0.017107</td></tr>
<tr><td rowspan="3">Tango Bottom Pose</td><td rowspan="3"><SUP>TBP</SUP><B>T</B><SUB>B</SUB></td><td>    0.9986085337</td><td>    0.0027720770</td><td>   -0.0526622452</td><td> -0.006998</td></tr>
<td>    0.0192066304</td><td>   -0.9491479460</td><td>    0.3142439848</td><td> -0.074008</td></tr>
<td>   -0.0491131534</td><td>   -0.3148181892</td><td>   -0.9478804808</td><td> -0.058834</td></tr>
</table>
