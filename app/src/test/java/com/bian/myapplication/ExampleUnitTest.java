package com.bian.myapplication;

import org.junit.Test;

import java.util.ArrayList;

import static org.junit.Assert.*;

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class ExampleUnitTest {
    private ArrayList<String> stringList;
    private ArrayList<Integer> integerList;

    @Test
    public void addition_isCorrect() {
        MyContainer<String> myContainer=new MyContainer<>();
        String msg = myContainer.data;
    }

    class MyContainer<T> {
        T data;
    }
}