package com.example.droidaudio

import android.annotation.SuppressLint
import android.graphics.Color
import android.graphics.drawable.GradientDrawable
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.sax.Element
import android.view.MotionEvent
import android.widget.TextView
import android.view.View
import androidx.constraintlayout.widget.ConstraintLayout
import com.example.droidaudio.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    @SuppressLint("ClickableViewAccessibility")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.sampleText.text = "Touch screen to play synth."

        binding.root.setOnTouchListener { view, event ->
            val x = event?.x
            val y = event?.y
            val width = view.width
            val height = view.height

            // Update synth params by passing x,y position to native methods.
            when (event?.action) {
                MotionEvent.ACTION_DOWN -> {
                    toggleSound()
                    setFilterCutoff(x as Float, width)
                    setFreq(y as Float, height)
                    val hexCodes = getSpectralHexes()
                    setGradient(view, hexCodes)
                    true
                }
                MotionEvent.ACTION_MOVE -> {
                    setFilterCutoff(x as Float, width)
                    setFreq(y as Float, height)
                    val hexCodes = getSpectralHexes()
                    setGradient(view, hexCodes)
                    true
                }
                MotionEvent.ACTION_UP -> {
                    toggleSound()
                    true
                }
                else -> {
                    println("else")
                    false
                }
            }
        }
    }

    private fun setGradient(view: View, hexCodes: Array<String>) {
        val drawable = GradientDrawable(GradientDrawable.Orientation.RIGHT_LEFT,
            intArrayOf(
                Color.parseColor("#FF${hexCodes[0]}"),
                Color.parseColor("#FF${hexCodes[1]}"),
                Color.parseColor("#FF${hexCodes[2]}")
            ))
        view.background = drawable
    }

    /**
     * Native methods that are implemented by the 'droidaudio' native library.
     */
    private external fun toggleSound(): Int
    private external fun setFreq(value: Float, screenWidth: Int): Int
    private external fun setFilterCutoff(value: Float, screenHeight: Int): Int
    private external fun getSpectralHexes(): Array<String>

    companion object {
        // Used to load the 'droidaudio' library on application startup.
        init {
            System.loadLibrary("droidaudio")
        }
    }
}